#include "Decoder.h"

#ifndef HARDWARE_DECODE


#include "DataBuf.h"
extern "C" 
{
	#include <libavcodec/avcodec.h>
}
#pragma comment(lib, "avcodec.lib")
#include <thread>
#include <QDebug>

Decoder::Decoder(Type type)
	:_type(type)
{
	run();
}

Decoder::~Decoder()
{
	close();
	finish();
}

void Decoder::run()
{
	this->finish();
	_work_thread = std::thread([this] {
		auto deleter = [](AVFrame* frame) { if (frame) av_frame_free(&frame); };
		_shutdown = false;
		while (!_shutdown)
		{
			//if (!_codec_ctx) //WAIT_AND_PASS(5); //这个好像有问题，就是程序运行起来打开的第一个视频要过很久才有画面
			//if(_type == Type::NONE_DECODE) WAIT_AND_PASS(5); 也有问题，所以说PacketPtr pkt{ nullptr };之前不能有sleep
			//printf("11qqqqqq");
			PacketPtr pkt{ nullptr };
			/** static int64_t master_timestamp{ 0 }; **/
			if (_type == Type::AUDIO_DECODE)
			{
				DataBuf::APacketQueue.dequeue(pkt);
				if (!this->send(pkt.get()))
					continue;
				while (!_shutdown)
				{
					AVFrame* frame = this->recv();
					if (!frame) break;
					/** master_timestamp = frame->pts; **/
					DataBuf::AFrameQueue.enqueue(FramePtr(frame, deleter));
				}
			}
			else if (_type == Type::VIDEO_DECODE)
			{
				DataBuf::VPacketQueue.dequeue(pkt);
				if (!this->send(pkt.get()))
					//std::this_thread::yield();
					continue;
				/*static long long pkt_num = 0;
				printf("                                 video: %ld %d %d %d\n", ++pkt_num, 
					pkt->duration, _codec_ctx->time_base.den, _codec_ctx->time_base.num);*/
				while (!_shutdown)
				{
					AVFrame* frame = this->recv();
					if (!frame) break;
					/** if (auto diff = frame->pts - master_timestamp; diff > 0)
					{
						//deleter(frame);
						//continue;
					} **/
					DataBuf::VFrameQueue.enqueue(FramePtr(frame, deleter));
				}
			}
		}
	});
}

void Decoder::finish()
{
	if (!_shutdown)
	{
		_shutdown = true;
		if (_work_thread.joinable())
			_work_thread.join();
	}
}

bool Decoder::open(AVCodecParameters* para, AVRational* timebase)
{
	if (!para) return false;
	this->close();
	std::lock_guard<std::mutex> lck(_mutex);
	//找到解码器
	AVCodec* codec = avcodec_find_decoder(para->codec_id);
	if (!codec)
	{
		//avcodec_parameters_free(&para);
		return false;
	}
	//创建解码器上下文
	_codec_ctx = avcodec_alloc_context3(codec);
	//配置解码器上下文参数
	avcodec_parameters_to_context(_codec_ctx, para);
	//avcodec_parameters_free(&para);
	//配置解码线程数
	_codec_ctx->thread_count = std::thread::hardware_concurrency(); ///8??
	//配置解码器时间基
	if(timebase) _codec_ctx->pkt_timebase = *timebase;
	//if (timebase) av_codec_set_pkt_timebase(_codec_ctx, *timebase);
	//打开解码器
	int ret = avcodec_open2(_codec_ctx, nullptr, nullptr);
	if (ret != 0)
	{
		avcodec_free_context(&_codec_ctx);
		return false;
	}
	return true;
}

void Decoder::close()
{
	std::lock_guard<std::mutex> lck(_mutex);
	if (!_codec_ctx) return;
	avcodec_flush_buffers(_codec_ctx);
	avcodec_close(_codec_ctx);
	avcodec_free_context(&_codec_ctx); //最终定位到这边，这边调用free导致了音频播放线程的swr_convert崩溃，
	                                   //可能是那时候swr_convert处理的AVFrame是个脏帧
	//qDebug() << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";未输出
	_seek_target = 0;
}

bool Decoder::send(AVPacket* pkt)
{
	if (!pkt || pkt->size <= 0 || !pkt->data)
		return false;
	std::lock_guard<std::mutex> lck(_mutex);
	if (!_codec_ctx) return false;
	int ret = avcodec_send_packet(_codec_ctx, pkt);
	/* av_packet_free(&pkt); */
	return ret == 0 ? true : false;
}

AVFrame* Decoder::recv()
{
	std::lock_guard<std::mutex> lck(_mutex);
	if (!_codec_ctx) return nullptr;
	AVFrame* frame = av_frame_alloc();
	if (int ret = avcodec_receive_frame(_codec_ctx, frame); ret != 0)
	{
		av_frame_free(&frame); //2020.01.06目前用的FFmpeg API缺少avi格式对应的软解码器(硬解码可以)，
		                       //导致此类格式视频文件有声音无画面
		//printf("empty %d %d\n", ret, AVERROR(EAGAIN)); //有时会出现一直avcodec_receive_frame不到数据的情况。
		return nullptr;
	}
	//frame->pts *= av_q2d(_codec_ctx->pkt_timebase) * 1000; //20201227
	//printf("%f ", av_q2d(_codec_ctx->pkt_timebase));
	/////////////////////////////////////
	//seek模式为查找关键帧，所以上个最近关键帧到实际seek位置这段时间内不应显示出画面
	int64_t pts_ms = frame->pts * av_q2d(_codec_ctx->pkt_timebase) * 1000;
	if (_type == Type::VIDEO_DECODE && pts_ms < _seek_target)
	{
		av_frame_free(&frame);
		return nullptr;
	}
	/////////////////////////////////////
	current = pts_ms;
	return frame;
}

void Decoder::seek(int64_t target)
{
	std::lock_guard<std::mutex> lck(_mutex);
	_seek_target = target;
	//avcodec_flush_buffers(_codec_ctx);不能调用
}

#endif
