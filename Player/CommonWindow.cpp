#include "CommonWindow.h"
#include "BaseWindow.h"

/*��������NoFocus���ԣ����¿ո���ͻᴥ����ǰӵ��focus�İ�ť�ĵ���ź�*/

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
	/* delete _base; �����*/
}

void CommonWindow::setVisible(bool flag)
{
	_base->setVisible(flag);
	QWidget::setVisible(flag);
}

void CommonWindow::show()
{
	_base->show();
	QWidget::show(); //base��close�����Լ�close���������showҪһ��show
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
	//�����ǰ�����޽�����޷���ʾ��tooltip������
	//���У����õ�
	//activateWindow();
}
