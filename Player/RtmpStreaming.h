#pragma once
#include <mutex>
#include <thread>
#include <atomic>
#include <functional>
#include "DataBuf.h"

struct AVPacket;
struct AVStream;
struct AVFormatContext;
struct AVCodecContext;
class PixelFmtConvert;
//class SampleFmtConvert;
//class Encoder;
class RtmpStreaming
{
public:
	RtmpStreaming();
	~RtmpStreaming();
	void run();
	void finish();
	bool init(const char* url);
	void deinit();
	void setConnErrCB(const std::function<void()>& cb) {
		_conn_err_cb = cb;
	}

private:
	bool sendHead();
	bool sendFrame(AVPacket*, int stream_index);

private:
	std::string _url{ "" };
	std::mutex _mtx;
	std::thread _work_thread;
	std::atomic_bool _shutdown{ false };
	std::function<void()> _conn_err_cb;
	//Encoder* _vencoder{ nullptr };
	//Encoder* _aencoder{ nullptr };
	//PixelFmtConvert* _vconverter{ nullptr };
	//SampleFmtConvert* _aconverter{ nullptr };
	AVCodecContext* _ac{ nullptr };
	AVCodecContext* _vc{ nullptr };
	AVStream* _as{ nullptr };
	AVStream* _vs{ nullptr };
	AVFormatContext* _ofmtctx{ nullptr };
};

