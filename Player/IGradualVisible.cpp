#include "IGradualVisible.h"
#include "Animation.h"


IGradualVisible::IGradualVisible(QWidget* owner)
	:_owner(owner), _timer(new QTimer(owner)),
	_opacity(new QGraphicsOpacityEffect(owner)),
	_show_animation(new Animation(_opacity, "opacity", owner)),
	_hide_animation(new Animation(_opacity, "opacity", owner))
{
	_opacity->setOpacity(1.);
	_owner->setGraphicsEffect(_opacity);
	_show_animation->setStartValue(0.);
	_show_animation->setEndValue(1.);
	_hide_animation->setStartValue(1.);
	_hide_animation->setEndValue(0.);
	_show_animation->setEasingCurve(QEasingCurve::Linear);
	_hide_animation->setEasingCurve(QEasingCurve::Linear);
	_timer->setSingleShot(true);
	QObject::connect(_timer, &QTimer::timeout, [this] { disappear(); });
}

bool IGradualVisible::appear(int msec)
{
	if (_show_animation->isActive() || _opacity->opacity() == 1.)
		return false;
	_show_animation->setDuration(msec);
	_show_animation->start();
	return true;
}

bool IGradualVisible::disappear(int msec)
{
	if (_hide_animation->isActive() || _opacity->opacity() == 0.)
		return false;
	_hide_animation->setDuration(msec);
	_hide_animation->start();
	return true;
}

void IGradualVisible::autoDisappear(int timeout)
{
	_timer->stop();
	_timer->start(timeout);
}

//bool IGradualVisible::visible() const
//{
//	return (_opacity->opacity() == 1.);
//}
