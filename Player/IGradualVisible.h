/**
* @file IGradualVisible.h
* @brief 渐变可视化接口
* @authors yandaoyang
* @date 2020/12/30
* @note
*/

#pragma once
#include <QTimer>
#include <QWidget>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

class Animation;
class IGradualVisible
{
public:
	explicit IGradualVisible(QWidget* owner);
	virtual ~IGradualVisible() = default;

public:
	/*void setShowDuration(int msec);
	void setHideDuration(int msec);
	void setHideTimeout(int msec);*/
	virtual bool appear(int msec = 1000);
	virtual bool disappear(int msec = 1000);
	virtual void autoDisappear(int timeout = 3000);
	//bool visible() const;

protected:
	QWidget                *_owner         { nullptr };
	QTimer                 *_timer         { nullptr };
	QGraphicsOpacityEffect *_opacity       { nullptr };
	Animation              *_show_animation{ nullptr };
	Animation              *_hide_animation{ nullptr };
};
