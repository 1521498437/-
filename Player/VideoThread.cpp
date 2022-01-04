#include "VideoThread.h"
#include "IVideoCall.h"
#include "DataBuf.h"
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/time.h>
}
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "opencv_world3413.lib")
#pragma comment(lib, "opencv_world3413d.lib")
#include "PixelFmtConvert.h"
#include "MediaEncoder.h"
#include "AudioThread.h" //////////////
#include "MatFilterNet.h"
#include "MediaController.h"


VideoThread::VideoThread(QObject* p)
	:QThread(p)
{
	start();
}

VideoThread::~VideoThread()
{
	stop(); //QThread::yieldCurrentThread();
}

void VideoThread::setCall(IVideoCall* call, AVCodecParameters* para)
{
	if (!call || !para) return;
	QMutexLocker lck(&_mutex);
	_call = call;
	///_call->init(para->width, para->height);//转移converter
	DELETE_PTR(_converter); //考虑加入定位new
	_converter = new PixelFmtConvert(para->width, para->height, AV_PIX_FMT_RGB24);//宽、高... 全局统一

	//设定推流参数
	//if()
	MediaEncoder::Get()->inWidth = para->width;
	MediaEncoder::Get()->inHeight = para->height;
	MediaEncoder::Get()->outWidth = para->width;
	MediaEncoder::Get()->outHeight = para->height;
	MediaEncoder::Get()->reset();
	MediaEncoder::Get()->initScale();
	MediaEncoder::Get()->initVideoCodec();

	//avcodec_parameters_free(&para);
	_continue = false;
	_pause = false;
}

void VideoThread::stop()
{
	if (!_shutdown)
	{
		_shutdown = true;
		wait();
	}
}

//void VideoThread::seek(int64_t target)
//{
//	if (!_pause) return;
//	/*_seek = true;
//	_seek_target = target;*/
//	MEDIA_CTL->resume();
//	QTimer::singleShot(10, [this] { MEDIA_CTL->suspend(); });
//}

void VideoThread::run()
{
	_shutdown = false;
	while (!_shutdown)
	{
		if (_pause)
		{
			/*if (_seek)
				DataBuf::VFrameQueue.clear();
			else*/
			{
				QThread::msleep(5);
				continue;
			}
		}
		FramePtr frame{ nullptr };
		if (DataBuf::VFrameQueue.dequeue(frame)) //会阻塞
		{
			QMutexLocker lck(&_mutex);
			//if (_shutdown) break;
			if (_continue)
			{
				QThread::msleep(3);
				continue;
			}
			//if (frame.get())
			//{
#ifdef AVSYNC
			int64_t diff_ms = frame->pts - AudioThread::MasterTimestamp;
			if (diff_ms > 0)
			{
				QThread::msleep(1);
				continue;
			}
#endif
			///_call->refresh(frame.get());//转移converter
			if (_converter->cvtToBuf(frame.get(), SWS_FAST_BILINEAR))
			{
				uchar* rgb = _converter->getOutBuf();
				int w = _converter->width(), h = _converter->height();
				//printf("%d     %d\n", w, h);640 480

				if (MEDIA_CTL->curPlayingType() == MediaController::DEVICE)
				{
					//美颜
					cv::Mat mat(h, w, CV_8UC3, rgb);
					MatFilterNet::Get()->filter(mat);

					//推流队列
					AVData* data = new AVData((char*)mat.data, mat.cols * mat.rows * mat.elemSize(), av_gettime());
					DataBuf::VStreamingQueue.enqueue(DataPtr(data), false);

					//渲染显示
					_call->refresh(mat.data, w, h);
				}
				else
				{
					_call->refresh(rgb, w, h);
				}
			}
				/*//static int n = 0;
				if (_pause) printf("refresh %d\n", ++n);
				if (_seek && frame->pts >= _seek_target)
				{
					_seek = false;
					_seek_target = 0;
				}
			//}*/
		}
	}
}
