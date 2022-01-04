#include "TitleBar.h"
#include "Util.h"
#include "MediaController.h"
#pragma execution_character_set("utf-8")
#include <QDebug>

TitleBar::TitleBar(QWidget* parent)
	: QWidget(parent), IGradualVisible(this), 
	_isMax(false),
	_menu(new QMenu(this))
{
	ui.setupUi(this);
	QFont font;
	font.setBold(true);
	font.setPointSize(9);
	font.setFamily("youyuan");
	_menu->setFont(font);
	_menu->addAction("打开本地视频", this, &TitleBar::sigOpenFile);
	_device_menu = _menu->addMenu("打开摄像头");
	_menu->addAction("打开直播面板", this, &TitleBar::sigRtmpStreaming);
}

TitleBar::~TitleBar()
{
}

void TitleBar::init()
{
	Util::setButtonIcon(ui.btnMin, QChar(0xf2d1), 9);
	Util::setButtonIcon(ui.btnMax, QChar(0xf2d0), 9);
	Util::setButtonIcon(ui.btnClose, QChar(0xf00d), 13);
	setStyleSheet(Util::loadQssFile(":/App/Res/qss/TitleBar.css"));
	connect(ui.btnMin, &QPushButton::clicked, this, &TitleBar::sigShowMin);
	connect(ui.btnMax, &QPushButton::clicked, this, &TitleBar::onBtnMaxClicked);
	connect(ui.btnClose, &QPushButton::clicked, this, &TitleBar::sigCloseWin);

	connect(ui.btnMainMenu, &QPushButton::pressed, this, &TitleBar::sigOpenCamera);
	ui.btnMainMenu->setMenu(_menu);
}

int TitleBar::buttonsWidth() const
{
	return ui.btnMin->width() + ui.btnMax->width() + ui.btnClose->width();
}

void TitleBar::setMinimizable(bool able)
{
	able ? ui.btnMin->show() : ui.btnMin->hide();
}

void TitleBar::setMaximizable(bool able)
{
	_maximizable = able ? true : false;
	able ? ui.btnMax->show() : ui.btnMax->hide();
}

bool TitleBar::disappear(int msec)
{
	//如果鼠标停留在本身范围内或者没有视频正在播放，就不消失
	QPoint pos = QCursor::pos();
	if (pos.x() >= _globalPos.x() && pos.x() <= _globalPos.x() + width() &&
		pos.y() >= _globalPos.y() && pos.y() <= _globalPos.y() + height())
		return false;
	return MEDIA_CTL->isPlaying() ? IGradualVisible::disappear(msec) : false;
}

void TitleBar::onBtnMaxClicked()
{
	if (!_isMax)
	{
		emit sigShowMax();
		//缺少还原图标
		ui.btnMax->setTipText("还原");
		ui.btnMax->setText(QChar(0xf2d2));
	}
	else
	{
		emit sigRestore();
		ui.btnMax->setTipText("最大化");
		ui.btnMax->setText(QChar(0xf2d0));
	}
	_isMax = !_isMax;
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent*)
{
	if (_maximizable) onBtnMaxClicked();
}
