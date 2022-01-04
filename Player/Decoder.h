/**
* @file Decoder.h
* @brief (Èí)½âÂëÆ÷
* @authors yandaoyang
* @date 2020/12/09
* @note
*/

#pragma once
#include <thread>
#include <mutex>
#include "PubDef.h"

struct AVFrame;
struct AVPacket;
struct AVRational;
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
	Decoder(Type type);
	~Decoder();
	

public:
	void run();
	void finish();

public:
	bool open(AVCodecParameters*, AVRational*);
	void close();
	bool send(AVPacket*);
	AVFrame* recv();
	void seek(int64_t);
	int64_t current{ 0 };

private:
	AVCodecContext* _codec_ctx{ nullptr };
	Type            _type     { Type::NONE_DECODE };
	int64_t         _seek_target{ 0 };

private:
	std::mutex      _mutex;
	std::thread     _work_thread;
	bool            _shutdown { false };
};
