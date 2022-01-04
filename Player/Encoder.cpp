#include "Encoder.h"
extern "C" {
#include <libavcodec/avcodec.h>
}
#pragma comment(lib, "avcodec.lib")
#include <thread>
#include <iostream>
using namespace std;

bool Encoder::initVideoCodec()
{
	if (!(vctx = createCodec(false))) return false;
	vctx->bit_rate = 50 * 1024 * 8;//压缩后每秒视频的bit位大小 50kB
	vctx->width = 1280;
	vctx->height = 720;
	//vc->time_base = { 1,25 };
	vctx->framerate = { 25,1 };
	//画面组的大小，多少帧一个关键帧
	vctx->gop_size = 50;
	vctx->max_b_frames = 0;
	vctx->pix_fmt = AV_PIX_FMT_YUV420P;
	return openCodec(&vctx);
}

bool Encoder::initAudioCodec()
{
	if (!(actx = createCodec(true))) return false;
	actx->bit_rate = 40000;
	actx->sample_rate = 44100;
	actx->channels = 2;
	actx->channel_layout = av_get_default_channel_layout(2);
	actx->sample_fmt = AV_SAMPLE_FMT_FLTP;
	return openCodec(&actx);
}

AVPacket* Encoder::encodeVideo(AVFrame* frame)
{
	if (!frame->data) return nullptr;
	int ret = avcodec_send_frame(vctx, frame);
	if (ret != 0) return nullptr;
	AVPacket* pkt = av_packet_alloc();
	ret = avcodec_receive_packet(vctx, pkt);
	if (ret != 0)
	{
		av_packet_free(&pkt);
		return nullptr;
	}
	return pkt;
}

AVPacket* Encoder::encodeAudio(AVFrame* frame)
{
	if (!frame->data) return nullptr;
	if (last_audio_pts == frame->pts)
		frame->pts += 1000;
	last_audio_pts = frame->pts;
	int ret = avcodec_send_frame(actx, frame);
	if (ret != 0) return nullptr;
	AVPacket* pkt = av_packet_alloc();
	ret = avcodec_receive_packet(actx, pkt);
	if (ret != 0)
	{
		av_packet_free(&pkt);
		return nullptr;
	}
	return pkt;
}

AVCodecContext* Encoder::createCodec(bool isAudio)
{
	AVCodec* codec = avcodec_find_encoder(isAudio ? AV_CODEC_ID_AAC : AV_CODEC_ID_H264);
	if (!codec)
	{
		cout << "avcodec_find_encoder  failed!" << endl;
		return nullptr;
	}
	AVCodecContext* c = avcodec_alloc_context3(codec);
	if (!c)
	{
		cout << "avcodec_alloc_context3  failed!" << endl;
		return nullptr;
	}
	c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
	c->thread_count = std::thread::hardware_concurrency();
	c->time_base = { 1,1000000 };
	return c;
}

bool Encoder::openCodec(AVCodecContext** c)
{
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