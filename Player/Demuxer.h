/**
* @file Demuxer.h
* @brief 解复用器
* @authors yandaoyang
* @date 2020/12/09
* @note
*/

#pragma once
#include <thread>
#include <mutex>
#include <tuple>
#include <ctime>
#include <map>
#include <condition_variable>
#include <functional>
namespace std { using cond = std::condition_variable; }
using ErrHandlerMap = std::map<int, std::function<void(void)>>;

struct AVPacket;
struct AVRational;
struct AVDictionary;
struct AVFormatContext;
struct AVCodecParameters;
class Demuxer
{
public:
	Demuxer();
	~Demuxer();

public:
	void run();                                              /* 线程启动运行 */
	void close();                                            /* 关闭媒体流 */
	void finish();                                           /* 线程结束运行 */
	AVPacket* read();                                        /* 读取媒体流 */
	bool open(const char* url, bool isDevice = false, int timeout = 3);       /* 打开媒体流 */
	bool seek(double pos);
	std::tuple<AVCodecParameters*, AVRational*> copyVPara(); /* 复制视频流参数 */
	std::tuple<AVCodecParameters*, AVRational*> copyAPara(); /* 复制音频流参数 */
	void registerErrorHandler(int, const std::function<void(void)>&); /* 注册错误处理回调 */
	void pause(bool);

public:
	//int64_t          current { 0 };
	int64_t          duration{ 0 };

private:
	void setOpenTimeout(int seconds);
	void handleReadError(int errnum);

private:
	::time_t         _lasttime{ 0 };
	int              _vindex  { -1 };
	int              _aindex  { -1 };
	AVDictionary    *_opts    { nullptr };
	AVFormatContext *_fmt_ctx { nullptr };
	ErrHandlerMap    _err_handler;

private:
	std::cond        _resume_cond;
	std::mutex       _resume_mutx;
	bool             _resume{ true };

private:
	std::mutex       _mutex;
	std::thread      _work_thread;
	bool             _shutdown{ false };
};
