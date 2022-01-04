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
			//if (!_codec_ctx) //WAIT_AND_PASS(5); //������������⣬���ǳ������������򿪵ĵ�һ����ƵҪ���ܾò��л���
			//if(_type == Type::NONE_DECODE) WAIT_AND_PASS(5); Ҳ�����⣬����˵PacketPtr pkt{ nullptr };֮ǰ������sleep
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
	//�ҵ�������
	AVCodec* codec = avcodec_find_decoder(para->codec_id);
	if (!codec)
	{
		//avcodec_parameters_free(&para);
		return false;
	}
	//����������������
	_codec_ctx = avcodec_alloc_context3(codec);
	//���ý����������Ĳ���
	avcodec_parameters_to_context(_codec_ctx, para);
	//avcodec_parameters_free(&para);
	//���ý����߳���
	_codec_ctx->thread_count = std::thread::hardware_concurrency(); ///8??
	//���ý�����ʱ���
	if(timebase) _codec_ctx->pkt_timebase = *timebase;
	//if (timebase) av_codec_set_pkt_timebase(_codec_ctx, *timebase);
	//�򿪽�����
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
	avcodec_free_context(&_codec_ctx); //���ն�λ����ߣ���ߵ���free��������Ƶ�����̵߳�swr_convert������
	                                   //��������ʱ��swr_convert�����AVFrame�Ǹ���֡
	//qDebug() << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";δ���
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
		av_frame_free(&frame); //2020.01.06Ŀǰ�õ�FFmpeg APIȱ��avi��ʽ��Ӧ���������(Ӳ�������)��
		                       //���´����ʽ��Ƶ�ļ��������޻���
		//printf("empty %d %d\n", ret, AVERROR(EAGAIN)); //��ʱ�����һֱavcodec_receive_frame�������ݵ������
		return nullptr;
	}
	//frame->pts *= av_q2d(_codec_ctx->pkt_timebase) * 1000; //20201227
	//printf("%f ", av_q2d(_codec_ctx->pkt_timebase));
	/////////////////////////////////////
	//seekģʽΪ���ҹؼ�֡�������ϸ�����ؼ�֡��ʵ��seekλ�����ʱ���ڲ�Ӧ��ʾ������
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
	//avcodec_flush_buffers(_codec_ctx);���ܵ���
}

#endif
