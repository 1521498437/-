/**
* @file Demuxer.h
* @brief �⸴����
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
	void run();                                              /* �߳��������� */
	void close();                                            /* �ر�ý���� */
	void finish();                                           /* �߳̽������� */
	AVPacket* read();                                        /* ��ȡý���� */
	bool open(const char* url, bool isDevice = false, int timeout = 3);       /* ��ý���� */
	bool seek(double pos);
	std::tuple<AVCodecParameters*, AVRational*> copyVPara(); /* ������Ƶ������ */
	std::tuple<AVCodecParameters*, AVRational*> copyAPara(); /* ������Ƶ������ */
	void registerErrorHandler(int, const std::function<void(void)>&); /* ע�������ص� */
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
