#include "CommonWindow.h"
#include "BaseWindow.h"

/*若不设置NoFocus属性，按下空格键就会触发当前拥有focus的按钮的点击信号*/

CommonWindow::CommonWindow(QWidget* parent)
	: QWidget(parent), _base(new BaseWindow(this))
{
	//setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
	//_base->addContentWidget(this);
	connect(_base, &BaseWindow::sigClose, this, &QWidget::close);
	setAttribute(Qt::WA_AlwaysShowToolTips);
}

CommonWindow::~CommonWindow()
{
	/* delete _base; 会崩溃*/
}

void CommonWindow::setVisible(bool flag)
{
	_base->setVisible(flag);
	QWidget::setVisible(flag);
}

void CommonWindow::show()
{
	_base->show();
	QWidget::show(); //base的close带动自己close，所以这边show要一起都show
}

void CommonWindow::hide()
{
	_base->hide();
}

void CommonWindow::close()
{
	_base->close();
}

void CommonWindow::move(int w, int h)
{
	_base->move(w, h);
}

void CommonWindow::resize(int w, int h)
{
	_base->resize(w, h);
}

void CommonWindow::setResizable(bool flag)
{
	_base->setResizable(flag);
}

void CommonWindow::setMinimizable(bool flag)
{
	_base->titleBar()->setMinimizable(flag);
}

void CommonWindow::setMaximizable(bool flag)
{
	_base->titleBar()->setMaximizable(flag);
}

void CommonWindow::setWindowFlags(Qt::WindowFlags flag)
{
	_base->setWindowFlags(_base->windowFlags() | flag);
}

Qt::WindowFlags CommonWindow::windowFlags() const
{
	return _base->windowFlags();
}

int CommonWindow::shadowWidth() const
{
	return _base->shadowWidth();
}

TitleBar* CommonWindow::titleBar() const
{
	return _base->titleBar();
}

const QRect& CommonWindow::geometry() const
{
	return _base->geometry();
}

void CommonWindow::setGeometry(int ax, int ay, int aw, int ah)
{
	_base->setGeometry(ax, ay, aw, ah);
}

void CommonWindow::handleNotify(const QString&, const QVariant&)
{
}

void CommonWindow::enterEvent(QEvent* e)
{
	QWidget::enterEvent(e);
	//解决当前窗口无焦点就无法显示出tooltip的问题
	//不行，会置底
	//activateWindow();
}
