/**
* @file Animation.h
* @brief 自定义属性动画类 
* @authors yandaoyang
* @date 2020/12/30
* @note
*/

#pragma once
#include <QPropertyAnimation>

class Animation : public QPropertyAnimation
{
	Q_OBJECT

public:
	Animation(QObject* target, const QByteArray& propertyName, QObject* parent = nullptr);
	~Animation() = default;

public:
	void start();
	void stop();
	bool setUnMutex() { _is_mutex == false; }
	bool isActive() const { return _is_active; }

private:
	bool _is_mutex { true };
	bool _is_active{ false };
};
