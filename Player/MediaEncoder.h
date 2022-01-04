#pragma once
#include "DataBuf.h"

struct AVCodecContext;
class MediaEncoder
{
public:
	//输入参数
	int inWidth = 640;
	int inHeight = 480;
	int inPixSize = 3;
	int channels = 2;
	int sampleRate = 44100;

	//输出参数
	int outWidth = 640;
	int outHeight = 480;
	int fps = 25;
	int bitrate = 50 * 1024 * 8;
	int nbSamples = 1024;

public:
	static MediaEncoder* Get();
	virtual ~MediaEncoder() = default;

public:
	virtual void reset() = 0;
	virtual bool initScale() = 0;
	virtual bool initResample() = 0;
	virtual AVData resample(AVData) = 0;
	virtual AVFrame* rgb2yuv(AVData*) = 0;

public:
	virtual bool initVideoCodec() = 0;
	virtual bool initAudioCodec() = 0;
	virtual AVPacket* encodeVideo(AVFrame*) = 0;
	virtual AVData encodeAudio(AVData) = 0;

public:
	AVCodecContext* vcodec;
	AVCodecContext* acodec;
};

