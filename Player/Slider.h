/**
* @file Slider.h
* @brief 自定义滑动条控件
* @authors yandaoyang
* @date 2020/12/23
* @note
*/

#pragma once
#include <QSlider>

class BubbleTip;
class Slider : public QSlider
{
	Q_OBJECT

public:
	explicit Slider(QWidget* parent = Q_NULLPTR);
	~Slider();

signals:
	void sigPressed();
	void sigReleased();
	void sigToolTip(double pos);

public:
	void setToolTip(const QString&);
	void setTipText(const QString&);
	QString toolTip() const;
	void setGlobalPos(const QPoint& p) { _globalPos = p; }
	//void setRealRange(int range) { _real_range = range; }

protected:
	bool eventFilter(QObject*, QEvent*) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent*) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent*) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent*) Q_DECL_OVERRIDE;

private:
	QPoint _globalPos;
	bool _is_enter{ false };
	bool _is_pressed{ false };
	//int _real_range{ 999 };
	BubbleTip* _tip{ nullptr };
};
