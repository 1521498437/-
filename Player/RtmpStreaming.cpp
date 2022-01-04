#include "RtmpStreaming.h"
extern "C" {
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/time.h>
}
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "opencv_world3413.lib")
#pragma comment(lib, "opencv_world3413d.lib")
#include <QDebug>
#include "DataBuf.h"
//#include "Encoder.h"
//#include "PixelFmtConvert.h"
//#include "SampleFmtConvert.h"
#include "MediaEncoder.h"

#define ERROR(ret) \
if (ret != 0) { \
	char buf[1024] = { 0 }; \
	av_strerror(ret, buf, sizeof(buf)); \
	qDebug() << buf; \
	return false; \
}

RtmpStreaming::RtmpStreaming()
{
}

RtmpStreaming::~RtmpStreaming()
{
	deinit();
	finish();
}

void RtmpStreaming::run()
{
	this->finish();
	_work_thread = std::thread([this] {
		_shutdown = false;
		if (!sendHead()) return;
		long long beginTime = av_gettime();
		while (!_shutdown)
		{
			/*AVData ad, vd;
			//DataBuf::AStreamingQueue.dequeue(ad);
			DataBuf::VStreamingQueue.dequeue(vd);
			//������Ƶ
			if (ad.size > 0)
			{
				ad.pts = ad.pts - beginTime;
				//�ز���
				//����
				//����
			}
			//������Ƶ
			if (vd.size > 0)
			{
				vd.pts = vd.pts - beginTime;
				//���ظ�ʽת��
				AVData yuv = MediaEncoder::Get()->rgb2yuv(vd);
				//����
				//AVData pkt = MediaEncoder::Get()->encodeVideo(yuv);
				//����
				//if (pkt.size > 0) sendFrame(pkt, _vs->index);
			}*/


			DataPtr vd{ nullptr };
			DataBuf::VStreamingQueue.dequeue(vd);
			if (vd && vd->size > 0)
			{
				vd->pts = vd->pts - beginTime;
				AVFrame* yuv = MediaEncoder::Get()->rgb2yuv(vd.get());
				AVPacket* pkt = MediaEncoder::Get()->encodeVideo(yuv);
				if (pkt && pkt->size > 0) sendFrame(pkt, _vs->index);
			}
		}
	});
}

void RtmpStreaming::finish()
{
	if (!_shutdown)
	{
		_shutdown = true;
		if (_work_thread.joinable())
			_work_thread.join();
	}
}

bool RtmpStreaming::init(const char* url)
{
	deinit();
	std::lock_guard<std::mutex> lck(_mtx);

	_ac = MediaEncoder::Get()->acodec;
	_vc = MediaEncoder::Get()->vcodec;

	int ret = avformat_alloc_output_context2(&_ofmtctx, nullptr, "flv", url);
	ERROR(ret);
	_url = url;
	if (!_ac && !_vc) return false;
	if (_ac)
	{
		_as = avformat_new_stream(_ofmtctx, nullptr);
		if (!_as) return false;
		_as->codecpar->codec_tag = 0;
		avcodec_parameters_from_context(_as->codecpar, _ac);
	}
	if (_vc)
	{
		_vs = avformat_new_stream(_ofmtctx, nullptr);
		if (!_vs) return false;
		_vs->codecpar->codec_tag = 0;
		avcodec_parameters_from_context(_vs->codecpar, _vc);
	}

	//if(!_vconverter) _vconverter = new PixelFmtConvert(AV_PIX_FMT_YUV420P);
	//if (!_aconverter) _aconverter = new SampleFmtConvert()
	return true;
}

void RtmpStreaming::deinit()
{
	std::lock_guard<std::mutex> lck(_mtx);
	if (_ofmtctx) avformat_close_input(&_ofmtctx);
	_ac = nullptr;
	_vc = nullptr;
	_as = nullptr;
	_vs = nullptr;
}

bool RtmpStreaming::sendHead()
{
	///��rtmp ���������IO
	int ret = avio_open(&_ofmtctx->pb, _url.c_str(), AVIO_FLAG_WRITE);
	if (ret != 0) {
		std::invoke(_conn_err_cb);
		return false;
	}
	//д���װͷ
	ret = avformat_write_header(_ofmtctx, NULL);
	ERROR(ret);
	return true;
}

bool RtmpStreaming::sendFrame(AVPacket* pkt, int stream_index)
{
	if (!pkt || !pkt->data) return false;
	//AVPacket* pack = (AVPacket*)d.data;
	pkt->stream_index = stream_index;
	AVRational stime;
	AVRational dtime;

	//�ж�����Ƶ������Ƶ
	if (_vs && _vc && pkt->stream_index == _vs->index)
	{
		stime = _vc->time_base;
		dtime = _vs->time_base;
	}
	else if (_as && _ac && pkt->stream_index == _as->index)
	{
		stime = _ac->time_base;
		dtime = _as->time_base;
	}
	else
	{
		return false;
	}

	//����
	pkt->pts = av_rescale_q(pkt->pts, stime, dtime);
	pkt->dts = av_rescale_q(pkt->dts, stime, dtime);
	pkt->duration = av_rescale_q(pkt->duration, stime, dtime);
	int ret = av_interleaved_write_frame(_ofmtctx, pkt);
	if (ret == 0)
	{
		printf("# ");
		return true;
	}
	return false;
}


