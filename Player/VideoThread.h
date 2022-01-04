/**
* @file VideoThread.h
* @brief 视频刷新线程
* @authors yandaoyang
* @date 2020/12/20
* @note
*/

#pragma once
#include <QThread>
#include <QMutex>
#include <atomic>

struct IVideoCall;
struct AVCodecParameters;
class PixelFmtConvert;
class VideoThread : public QThread
{
	Q_OBJECT

public:
	explicit VideoThread(QObject* p = Q_NULLPTR);
	~VideoThread();

public:
	void setCall(IVideoCall*, AVCodecParameters*);
	void setContinue(bool flag) { _continue = flag; }
	void stop();
	void pause(bool flag) { _pause = flag; }
	//void seek(int64_t);

protected:
	void run() Q_DECL_OVERRIDE;

private:
	QMutex           _mutex;
	std::atomic_bool _shutdown{ false };
	std::atomic_bool _continue{ false };
	std::atomic_bool _pause   { false };
	//std::atomic_bool _seek    { false };
	//int64_t          _seek_target{ 0 };
	IVideoCall*      _call    { nullptr };
	PixelFmtConvert* _converter{ nullptr };
};
