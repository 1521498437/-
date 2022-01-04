/**
* @file QPainterWnd.h
* @brief IVideoCall实现类
* @authors yandaoyang
* @date 2020/12/04
* @note 采用QPainter渲染显示
* 当前内存已安全，待升级，待考虑加锁问题
*/

#pragma once
#include <QMutex>
#include <QWidget>
#include "IVideoCall.h"

class QPainterWnd : public QWidget, public IVideoCall
{
	Q_OBJECT

public:
	explicit QPainterWnd(QWidget* p = Q_NULLPTR);
	~QPainterWnd();

signals:
	//void sigRequestUpdate();

protected:
	//void init(int width, int height) Q_DECL_OVERRIDE;
	void refresh(uchar* data, int w, int h) Q_DECL_OVERRIDE;
	void paintEvent(QPaintEvent*) Q_DECL_OVERRIDE;

private:
	QMutex          _mutex;
	QImage          _frame;
	///BaseFmtConvert* _converter         { nullptr };//转移converter
	double          _frame_aspect_ratio{ 1960. / 1080 };
};
