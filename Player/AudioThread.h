/**
* @file AudioThread.h
* @brief “Ù∆µ≤•∑≈œﬂ≥Ã
* @authors yandaoyang
* @date 2020/12/20
* @note 
*/

#pragma once
#include <QThread>
#include <QMutex>
#include <atomic>
//#define AVSYNC

struct IAudioCall;
struct AVCodecParameters;
class AudioThread : public QThread
{
	Q_OBJECT

public:
	explicit AudioThread(QObject* p = Q_NULLPTR);
	~AudioThread();

public:
#ifdef AVSYNC
	static inline int64_t MasterTimestamp{ 0 };
#endif
	void setCall(IAudioCall*, AVCodecParameters*);
	void setContinue() { _continue = true; } //∂™÷°
	void stop();
	void pause(bool flag) { _pause = flag; }

protected:
	void run() Q_DECL_OVERRIDE;

private:
	QMutex           _mutex;
	std::atomic_bool _shutdown{ false };
	std::atomic_bool _continue{ false };
	std::atomic_bool _pause   { false };
	IAudioCall*      _call    { nullptr };
};
