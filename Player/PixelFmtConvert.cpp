#include "PixelFmtConvert.h"
extern "C"
{
	#include <libavutil/frame.h>
	#include <libavcodec/avcodec.h>
	#include <libswscale/swscale.h>
}
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "swscale.lib")

static constexpr int TRUE { 1 };
static constexpr int FALSE{ 0 };


PixelFmtConvert::PixelFmtConvert(int width, int height, int dst_format)
	:_picture(nullptr, [](AVFrame* f) { if (f) av_frame_free(&f); })
{
	std::lock_guard<std::mutex> lck(_mutex);
	_picture.reset(av_frame_alloc());
	_dst_format = dst_format;
	if (width > 0 && height > 0)
	{
		_width = width;
		_height = height;
		initOutBuf(_width, _height);
	}
}

PixelFmtConvert::PixelFmtConvert(int dst_format)
	:_picture(nullptr, [](AVFrame* f) { if (f) av_frame_free(&f); })
{
	std::lock_guard<std::mutex> lck(_mutex);
	_picture.reset(av_frame_alloc());
	_dst_format = dst_format;
}

PixelFmtConvert::~PixelFmtConvert()
{
}

void PixelFmtConvert::initOutBuf(int width, int height)
{
	int nbytes = avpicture_get_size((AVPixelFormat)_dst_format, width, height);
	_out_buffer.reset(new uint8_t[nbytes]);
	avpicture_fill(
		(AVPicture*)_picture.get(), 
		_out_buffer.get(), 
		(AVPixelFormat)_dst_format, 
		width, height
	);
}

int PixelFmtConvert::cvtToBuf(AVFrame* frame, int algorithm)
{
	if (!frame || frame->width <= 0 || frame->height <= 0)
		return FALSE;
	std::lock_guard<std::mutex> lck(_mutex);
	if (!_out_buffer)
		initOutBuf(frame->width, frame->height);
	_sws = sws_getCachedContext(
		_sws, frame->width, frame->height,
		(AVPixelFormat)frame->format,
		frame->width, frame->height,
		(AVPixelFormat)_dst_format,
		algorithm, //SWS_FAST_BILINEAR, SWS_BICUBIC
		nullptr, nullptr, nullptr
	);
	if (!_sws) return FALSE;
	sws_scale(_sws, 
		frame->data, frame->linesize, 
		0, frame->height, 
		_picture->data, _picture->linesize);
	return TRUE;
}
