#include "Animation.h"

static Animation* Last = nullptr;

Animation::Animation(QObject* target, const QByteArray& propertyName, QObject* parent)
	:QPropertyAnimation(target, propertyName, parent)
{
	connect(this, &Animation::finished, [this] { _is_active = false; });
}

void Animation::start()
{
	if (_is_mutex)
	{
		if (Last && Last->isActive())
			Last->stop();
		Last = this;
	}
	QPropertyAnimation::start(KeepWhenStopped);
	_is_active = true;
}

void Animation::stop()
{
	QPropertyAnimation::stop();
	_is_active = false;
}
