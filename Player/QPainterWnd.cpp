#include "QPainterWnd.h"
#include "PixelFmtConvert.h"
#include "DataBuf.h"
extern "C" 
{
	#include <libavutil/frame.h>
	#include <libswscale/swscale.h>
}
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swscale.lib")
#include <QPainter>
#include <QDebug>


QPainterWnd::QPainterWnd(QWidget* p)
	: QWidget(p)
{
	/*connect(this, SIGNAL(sigRequestUpdate()), this, SLOT(update()), Qt::QueuedConnection);
	connect(this, "sigRequestUpdate", this, "update", Qt::QueuedConnection);*/
}

QPainterWnd::~QPainterWnd()
{
	///DELETE_PTR(_converter);//转移converter
}

/*///void QPainterWnd::init(int width, int height)
{
	DELETE_PTR(_converter); //考虑加入定位new
	_converter = new PixelFmtConvert(width, height, AV_PIX_FMT_RGB24);
}*/ //转移converter

void QPainterWnd::refresh(uchar* data, int width, int height)
{
	///if (!frame || !_converter) return;//转移converter
	if (!data || width <= 0 || height <= 0) return;
	///if (_converter->cvtToBuf(frame, SWS_FAST_BILINEAR))//转移converter
	{
		_mutex.lock();
		///_frame = QImage(_converter->getOutBuf(), frame->width, frame->height, //这边outbuf在多个线程被用到
			//QImage::/*Format_RGBA8888*/Format_RGB888).copy(0, 0, frame->width, frame->height); //转移converter

		_frame = QImage(data, width, height, QImage::Format_RGB888).copy(0, 0, width, height); //QImage的深拷贝与浅拷贝

		////按理来说以下入推流队列的操作应该放在VideoThread中，但_converter属于QPainterWnd，不得不妥协
		////这边写的耦合性就高了，后期再改正。
		////DataBuf::VStreamingQueue.enqueue(_frame.copy(0, 0, width, height), false);

		_frame_aspect_ratio = (double)width / (double)height;
		_mutex.unlock();
		QMetaObject::invokeMethod(this, "update", Qt::QueuedConnection);
	}
}

void QPainterWnd::paintEvent(QPaintEvent*)
{
	int width = this->width();
	int height = this->height();
	QPainter painter(this);
	painter.fillRect(0, 0, width, height, QColor(0,0,0,0.7));
	//设置抗锯齿，减少马赛克感
	//指示绘图引擎应该使用一个平滑pixmap转换算法(比如双线性插值)而不是最邻近插值算法
	painter.setRenderHint(QPainter::SmoothPixmapTransform);
	QMutexLocker lck(&_mutex);
	if (!_frame.isNull())
	{
		static QRectF target;
		double target_aspect_ratio = (double)width / (double)height;
		if (target_aspect_ratio == _frame_aspect_ratio)
		{
			target.setX(0);
			target.setY(0);
			target.setWidth(width);
			target.setHeight(height);
		}
		else if (target_aspect_ratio > _frame_aspect_ratio)
		{
			double frameW = _frame_aspect_ratio * height;
			target.setX(width / 2 - frameW / 2);
			target.setY(0);
			target.setWidth(frameW);
			target.setHeight(height);
		}
		else if (target_aspect_ratio < _frame_aspect_ratio)
		{
			double frameH = width / _frame_aspect_ratio;
			target.setX(0);
			target.setY(height / 2 - frameH / 2);
			target.setWidth(width);
			target.setHeight(frameH);
		}
		painter.drawImage(target, _frame, _frame.rect());
	}
}
