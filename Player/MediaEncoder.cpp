#include "MediaEncoder.h"
#include "DataBuf.h"
extern "C" {
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}
#include <mutex>
#include <thread>
#include <iostream>
using namespace std;
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swresample.lib")

class MediaEncoderImpl : public MediaEncoder
{
	SwsContext* vsc = nullptr;	//像素格式转换上下文
	SwrContext* asc = nullptr;	//音频重采样上下文
	AVFrame* yuv = nullptr;		//输出的YUV
	AVFrame* pcm = nullptr;		//重采样输出PCM
	AVPacket vpack = { 0 };		//视频帧
	AVPacket apack = { 0 };		//音频帧
	int vpts = 0;
	int apts = 0;
	std::mutex vmtx;
	std::mutex amtx;

public:
	~MediaEncoderImpl() {
		reset();
	}

protected:
	void reset() override 
	{
		{
			std::lock_guard<decltype(vmtx)> lck(vmtx);
			if (vsc) sws_freeContext(vsc);
			if (yuv) av_frame_free(&yuv);
			if (vcodec) avcodec_free_context(&vcodec);
			av_packet_unref(&vpack);
			vsc = nullptr;
			vpts = 0;
		}
		{
			std::lock_guard<decltype(amtx)> lck(amtx);
			if (asc) swr_free(&asc);
			if (pcm) av_frame_free(&pcm);
			if (acodec) avcodec_free_context(&acodec);
			av_packet_unref(&apack);
			apts = 0;
		}
	}

	bool initScale() override
	{
		std::lock_guard<decltype(vmtx)> lck(vmtx);
		//初始化格式转换上下文
		vsc = sws_getCachedContext(vsc,
			inWidth, inHeight, AV_PIX_FMT_RGB24,	 //源宽、高、像素格式
			outWidth, outHeight, AV_PIX_FMT_YUV420P,//目标宽、高、像素格式
			SWS_FAST_BILINEAR,  // 尺寸变化使用算法
			nullptr, nullptr, nullptr
		);
		if (!vsc)
		{
			cout << "sws_getCachedContext failed!" << endl;
			return false;
		}
		//初始化输出的数据结构
		yuv = av_frame_alloc();
		yuv->format = AV_PIX_FMT_YUV420P;
		yuv->width = inWidth;
		yuv->height = inHeight;
		yuv->pts = 0;
		//分配yuv空间
		int ret = av_frame_get_buffer(yuv, 32);
		if (ret != 0)
		{
			char buf[1024] = { 0 };
			av_strerror(ret, buf, sizeof(buf) - 1);
			cout << buf << endl;
			return false;
		}
		return true;
	}

	bool initResample() override
	{
		std::lock_guard<decltype(amtx)> lck(amtx);
		//初始化格式转换上下文
		asc = swr_alloc_set_opts(asc,
			av_get_default_channel_layout(channels), AV_SAMPLE_FMT_FLTP, sampleRate,//输出格式(这边写死了)
			av_get_default_channel_layout(channels), AV_SAMPLE_FMT_S16, sampleRate, 0, 0);//输入格式(这边写死了)
		if (!asc)
		{
			cout << "swr_alloc_set_opts failed!" << endl;
			return false;
		}
		int ret = swr_init(asc);
		if (ret != 0)
		{
			char err[1024] = { 0 };
			av_strerror(ret, err, sizeof(err) - 1);
			cout << err << endl;
			return false;
		}
		//音频重采样输出空间分配
		pcm = av_frame_alloc();
		pcm->format = AV_SAMPLE_FMT_FLTP;
		pcm->channels = channels;
		pcm->channel_layout = av_get_default_channel_layout(channels);
		pcm->nb_samples = nbSamples; //一帧音频一通道的采用数量
		ret = av_frame_get_buffer(pcm, 0); //给pcm分配存储空间
		if (ret != 0)
		{
			char err[1024] = { 0 };
			av_strerror(ret, err, sizeof(err) - 1);
			cout << err << endl;
			return false;
		}
		return true;
	}

	AVFrame* rgb2yuv(AVData* d) override
	{
		if (!d) return nullptr;
		std::lock_guard<decltype(vmtx)> lck(vmtx);
		///rgb to yuv
		//输入的数据结构
		uint8_t* indata[AV_NUM_DATA_POINTERS] = { 0 };
		//indata[0] bgrbgrbgr
		//plane indata[0] bbbbb indata[1]ggggg indata[2]rrrrr 
		indata[0] = (uint8_t*)d->data;
		int insize[AV_NUM_DATA_POINTERS] = { 0 };
		//一行（宽）数据的字节数
		insize[0] = inWidth * inPixSize;

		int h = sws_scale(vsc, indata, insize, 0, inHeight, //源数据
			yuv->data, yuv->linesize);
		if (h <= 0) return nullptr;
		yuv->pts = d->pts;
		return yuv;
	}

	AVData resample(AVData) override
	{
		std::lock_guard<decltype(amtx)> lck(amtx);
		return AVData(0,0,0);
	}

	bool initVideoCodec() override
	{
		std::lock_guard<decltype(vmtx)> lck(vmtx);
		//初始化编码上下文编码器
		if (!(vcodec = createCodec(AV_CODEC_ID_H264))) return false;
		vcodec->bit_rate = bitrate; //压缩后每秒视频的bit位大小 50kB
		vcodec->width = outWidth;
		vcodec->height = outHeight;
		//vc->time_base = { 1, fps };
		vcodec->framerate = { fps, 1 };
		//画面组的大小，多少帧一个关键帧
		vcodec->gop_size = 50;
		vcodec->max_b_frames = 0;
		vcodec->pix_fmt = AV_PIX_FMT_YUV420P;
		return openCodec(&vcodec);
	}

	bool initAudioCodec() override
	{
		std::lock_guard<decltype(amtx)> lck(amtx);
		if (!(acodec = createCodec(AV_CODEC_ID_AAC))) return false;
		acodec->bit_rate = 40000;
		acodec->sample_rate = sampleRate;
		acodec->sample_fmt = AV_SAMPLE_FMT_FLTP;
		acodec->channels = channels;
		acodec->channel_layout = av_get_default_channel_layout(channels);
		return openCodec(&acodec);
	}

	AVPacket* encodeVideo(AVFrame* frame) override
	{
		if (!frame || !frame->data) return nullptr;
		std::lock_guard<decltype(vmtx)> lck(vmtx);
		av_packet_unref(&vpack);
		//AVFrame* p = (AVFrame*)frame.data;

		///h264编码
		//frame->pts = vpts;
		//vpts++;
		int ret = avcodec_send_frame(vcodec, frame);
		if (ret != 0)
			return nullptr;

		ret = avcodec_receive_packet(vcodec, &vpack);
		if (ret != 0 || vpack.size <= 0)
			return nullptr;
		//r.data = (char*)&vpack;
		//r.size = vpack.size;
		//r.pts = frame.pts;
		vpack.pts = frame->pts;
		return &vpack;
	}

	AVData encodeAudio(AVData) override
	{
		std::lock_guard<decltype(amtx)> lck(amtx);
		return AVData(0,0,0);
	}

private:
	AVCodecContext* createCodec(AVCodecID cid)
	{
		//初始化编码器
		AVCodec* codec = avcodec_find_encoder(cid);
		if (!codec)
		{
			cout << "avcodec_find_encoder failed!" << endl;
			return nullptr;
		}
		//编码器上下文
		AVCodecContext* c = avcodec_alloc_context3(codec);
		if (!c)
		{
			cout << "avcodec_alloc_context3 failed!" << endl;
			return nullptr;
		}

		c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
		c->thread_count = std::thread::hardware_concurrency();
		c->time_base = { 1,1000000 };
		return c;
	}

	bool openCodec(AVCodecContext** c)
	{
		//打开编码器
		int ret = avcodec_open2(*c, 0, 0);
		if (ret != 0)
		{
			char err[1024] = { 0 };
			av_strerror(ret, err, sizeof(err) - 1);
			cout << err << endl;
			avcodec_free_context(c);
			return false;
		}
		return true;
	}
};

MediaEncoder* MediaEncoder::Get()
{
	static MediaEncoderImpl instance;
	return &instance;
}