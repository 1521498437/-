#include "AudioThread.h"
#include "IAudioCall.h"
#include "DataBuf.h"
extern "C"
{
#include <libavcodec/avcodec.h>
}
#pragma comment(lib, "avcodec.lib")
#include "VideoThread.h"


AudioThread::AudioThread(QObject* p)
	:QThread(p)
{
	start();
}

AudioThread::~AudioThread()
{
	stop();
}

void AudioThread::setCall(IAudioCall* call, AVCodecParameters* para)
{
	if (!call || !para) return;
	QMutexLocker lck(&_mutex);
	_call = call;
	_call->init(para->sample_rate, para->channels, para->format);
	//avcodec_parameters_free(&para); //这个还没解决呢 bug
	_continue = false;
	_pause = false;
}

void AudioThread::stop()
{
	if (!_shutdown)
	{
		_shutdown = true;
		wait();
	}
}

void AudioThread::run()
{
	while (!_shutdown)
	{
		if (_pause)
		{
			QThread::msleep(5);
			continue;
		}
		FramePtr frame{ nullptr };
		if (DataBuf::AFrameQueue.dequeue(frame)) //会阻塞
		{
			QMutexLocker lck(&_mutex);
			if (_continue)
			{
				QThread::msleep(3);
				continue;
			}
			//if (frame.get())
			//{
#ifdef AVSYNC
				MasterTimestamp = frame->pts - _call->noPlayMs();
#endif
				_call->write(frame.get());
			//}
		}
	}
}
