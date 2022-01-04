#pragma once

struct AVPacket;
struct AVFrame;
struct AVCodecContext;
class Encoder
{
public:
	bool initVideoCodec();
	bool initAudioCodec();
	AVPacket* encodeVideo(AVFrame*);
	AVPacket* encodeAudio(AVFrame*);
	void reset() { last_audio_pts = -1; }

	AVCodecContext* vctx{ nullptr };
	AVCodecContext* actx{ nullptr };
private:
	AVCodecContext* createCodec(bool isAudio);
	bool openCodec(AVCodecContext**);
	long long last_audio_pts{ -1 };
};

