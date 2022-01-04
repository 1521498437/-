#include "Slider.h"
#include <QMouseEvent>
#include "BubbleTip.h"

Slider::Slider(QWidget *parent)
	: QSlider(parent), _tip(new BubbleTip(this))
{
	setPageStep(0);
	setSingleStep(0);
	installEventFilter(this);
	setMouseTracking(true);
	setContentsMargins(0, 0, 0, 0);
}

Slider::~Slider()
{
}

void Slider::setToolTip(const QString& text)
{
	_tip->init(text);
}

void Slider::setTipText(const QString& text)
{
	_tip->setText(text);
}

QString Slider::toolTip() const
{
	return _tip->text();
}

bool Slider::eventFilter(QObject* obj, QEvent* e)
{
	if (obj == this)
	{
		if (e->type() == QEvent::ToolTip)
		{
			if (isEnabled())
			{
				if (!_is_enter)
				{
					int relativeX = QCursor::pos().x() - _globalPos.x()/*mapToGlobal(pos()).x()*/;
					double pos = (double)relativeX / (double)width();
					emit sigToolTip(pos);
				}
				_is_enter = true;
				_tip->popup();
			}
			return true;
		}
		else if (e->type() == QEvent::MouseMove)
		{
			if (_is_enter)
			{
				auto ev = (QMouseEvent*)e;
				double pos = (double)ev->pos().x() / (double)width();
				emit sigToolTip(pos);
				_tip->popup();
			}
		}
		else if (e->type() == QEvent::Leave)
		{
			_is_enter = false;
			_tip->hide();
		}
	}
	return QWidget::eventFilter(obj, e);
}

void Slider::mousePressEvent(QMouseEvent* e)
{
	QSlider::mousePressEvent(e);
	_is_pressed = true;
	double pos = (double)e->pos().x() / (double)width();
	int value = pos * (maximum() - minimum()) + minimum();
	setValue(value);
	emit this->sigPressed();
}

//bug: 点击滑动条上的非滑块区域滑动时触发不了此信号
void Slider::mouseMoveEvent(QMouseEvent* e)
{
	QSlider::mouseMoveEvent(e);
	if (_is_pressed)
	{
		double pos = (double)e->pos().x() / (double)width();
		int value = pos * (maximum() - minimum()) + minimum();
		setValue(value);
	}
}

void Slider::mouseReleaseEvent(QMouseEvent* e)
{
	QSlider::mouseReleaseEvent(e);
	_is_pressed = false;
	emit this->sigReleased();
}
