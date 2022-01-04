#include "Button.h"
#include <QEvent>
#include "BubbleTip.h"

Button::Button(QWidget* parent)
    :QPushButton(parent), _tip(new BubbleTip(this))
{
    installEventFilter(this);
}

Button::~Button()
{

}

void Button::setToolTip(const QString& text)
{
    _tip->init(text);
}

void Button::setTipText(const QString& text)
{
	_tip->setText(text);
}

QString Button::toolTip() const
{
    return _tip->text();
}

bool Button::eventFilter(QObject* obj, QEvent* e)
{
	if (obj == this)
	{
		if (e->type() == QEvent::ToolTip)
		{
			if(isEnabled()) _tip->popup();
           //_tip->move(x() + parentWidget()->x(), y() + parentWidget()->y() - height());
			return true;
		}
		else if (e->type() == QEvent::Leave)
			_tip->hide();
	}
	return QWidget::eventFilter(obj, e);
}
