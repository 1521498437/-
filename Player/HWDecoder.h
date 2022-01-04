/**
* @file HWDecoder.h
* @brief Ó²¼þ½âÂëÆ÷
* @authors yandaoyang
* @date 2020/12/27
* @note
*/

#pragma once
#include <thread>
#include <mutex>
extern "C" {
#include "libavutil/hwcontext.h"
}
#include "PubDef.h"

struct AVFrame;
struct AVCodec;
struct AVPacket;
struct AVRational;
struct AVBufferRef;
struct AVCodecContext;
struct AVCodecParameters;
class Decoder
{
public:
	enum class Type
	{
		AUDIO_DECODE,
		VIDEO_DECODE,
		NONE_DECODE
	};
	Decoder(Type);
	~Decoder();

public:
	void run();
	void finish();

public:
	bool open(AVCodecParameters*, AVRational*);
	void close();
	bool send(AVPacket*);
	AVFrame* recv();

private:
	void init(AVCodec*);
	AVHWDeviceType _hw_type{ AV_HWDEVICE_TYPE_NONE };
	AVBufferRef*   _hw_device_ctx{ nullptr };

private:
	AVCodecContext* _codec_ctx{ nullptr };
	Type            _type{ Type::NONE_DECODE };

private:
	std::mutex      _mutex;
	std::thread     _work_thread;
	bool            _shutdown{ false };
};
