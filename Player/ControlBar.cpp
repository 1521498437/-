#include "ControlBar.h"
#include "Util.h"
#include "MediaController.h"
#include "AppManager.h"
#include <qmessagebox.h>
#include "MainPlayer.h"
#include "QAudioPlay.h"
#pragma execution_character_set("utf-8")
#include <QDebug>

#define 播放 QChar(0xf04b)
#define 暂停 QChar(0xf04c)


ControlBar::ControlBar(QWidget* parent)
	: QWidget(parent), IGradualVisible(this),
	_moment_timer(new QTimer(this)),
	_slider_timer(new QTimer(this))
{
	ui.setupUi(this);
	//initWidgets();
	//bindSlots();
}

ControlBar::~ControlBar()
{
}

void ControlBar::init()
{
	Util::setButtonIcon(ui.btnPlayOrPause, QChar(0xf04b), 12);
	Util::setButtonIcon(ui.btnStop, QChar(0xf04d), 12);
	Util::setButtonIcon(ui.btnVolume, QChar(0xf028), 12);
	Util::setButtonIcon(ui.btnList, QChar(0xf036), 12);
	Util::setButtonIcon(ui.btnNext, QChar(0xf051), 12);
	Util::setButtonIcon(ui.btnPrev, QChar(0xf048), 12);
	Util::setButtonIcon(ui.btnSetting, QChar(0xf013), 12);
	setStyleSheet(Util::loadQssFile(":/App/Res/qss/ControlBar.css"));
	//ui.slider->setToolTip("00:00:00");//已在ui中
	ui.btnPlayOrPause->setEnabled(false); //这些考虑放在ui里
	ui.btnPlaySpeed->setEnabled(false);
	ui.slider->setEnabled(false);
	bindSlots();
}

void ControlBar::setGlobalPos(const QPoint& p)
{
	_globalPos = p;
	ui.slider->setGlobalPos(mapToGlobal(ui.slider->pos()));
}

bool ControlBar::disappear(int msec)
{
	//如果鼠标停留在本身范围内或者没有视频正在播放，就不消失
	QPoint pos = QCursor::pos();
	if(pos.x() >= _globalPos.x() && pos.x() <= _globalPos.x() + width() &&
		pos.y() >= _globalPos.y() && pos.y() <= _globalPos.y() + height())
		return false;
	return MEDIA_CTL->isPlaying() ? IGradualVisible::disappear(msec) : false;
}

void ControlBar::bindSlots()
{
	connect(MEDIA_CTL, &MediaController::sigReadyPlay, this, &ControlBar::onReadyPlay);
	connect(MEDIA_CTL, &MediaController::sigPlayEnd, this, &ControlBar::onPlayEnd);
	connect(_moment_timer, &QTimer::timeout, this, &ControlBar::onMomentUpdate);
	connect(_slider_timer, &QTimer::timeout, this, &ControlBar::onSliderUpdate);
	connect(ui.slider, &Slider::sigToolTip, this, &ControlBar::onSliderToolTip);
	connect(ui.slider, &Slider::sigPressed, _slider_timer, &QTimer::stop);
	connect(ui.slider, &Slider::sigReleased, this, &ControlBar::onVideoSeek);

	connect(ui.vslider, &QSlider::sliderReleased, [this] {
		AUDIO_PLAY->changeVolume((double)ui.vslider->value() / (double)ui.vslider->maximum());
	});
	connect(ui.vslider, &QSlider::sliderMoved, [this] {
		AUDIO_PLAY->changeVolume((double)ui.vslider->value() / (double)ui.vslider->maximum());
	});

	connect(this, &ControlBar::reqPlayOrPause, this, &ControlBar::onPlayOrPause);
	connect(ui.btnPlayOrPause, &QPushButton::clicked, this, &ControlBar::onPlayOrPause);
	connect(ui.btnStop, &QPushButton::clicked, this, &ControlBar::onStopPlay);
	connect(ui.btnPrev, &QPushButton::clicked, this, &ControlBar::onPrevMedia);
	connect(ui.btnNext, &QPushButton::clicked, this, &ControlBar::onNextMedia);
	connect(ui.btnPlaySpeed, &QPushButton::clicked, this, &ControlBar::onShowSpeed);
	connect(ui.btnVolume, &QPushButton::clicked, this, &ControlBar::onShowVolume);
	connect(ui.btnSetting, &QPushButton::clicked, this, &ControlBar::onShowSetting);
	connect(ui.btnList, &QPushButton::clicked, this, &ControlBar::onShowPlayList);
}

QTime ControlBar::msecToTime(int64_t msec)
{
	auto sec = msec / 1000;
	return QTime(sec / 3600, sec % 3600 / 60, sec % 60);
}

void ControlBar::onReadyPlay()
{
	//_isPlaying = true;
	_slider_timer->start(40);
	_moment_timer->start(1000);
	ui.playedEdt->setTime(QTime(0, 0, 0));
	ui.totalEdt->setTime(msecToTime(MEDIA_CTL->getTotalMsec()));
	ui.slider->setValue(ui.slider->minimum());
	ui.slider->setEnabled(true);
	ui.btnPlayOrPause->setTipText("暂停");
	ui.btnPlayOrPause->setText(暂停);
	ui.btnPlayOrPause->setEnabled(true);
	ui.btnPlaySpeed->setEnabled(true);
}

void ControlBar::onPlayEnd()
{
	//_isPlaying = false;
	_slider_timer->stop();
	_moment_timer->stop();
	ui.playedEdt->setTime(ui.totalEdt->time()); //待改进，相差1
	ui.slider->setValue(ui.slider->maximum());
	ui.slider->setEnabled(false);
	ui.btnPlayOrPause->setTipText("播放");
	ui.btnPlayOrPause->setText(播放);
	ui.btnPlayOrPause->setEnabled(false);
	ui.btnPlaySpeed->setEnabled(false);
}

void ControlBar::onMomentUpdate()
{
	ui.playedEdt->setTime(msecToTime(MEDIA_CTL->getPlayedMsec()));
}

void ControlBar::onSliderUpdate()
{
	double rate = (double)MEDIA_CTL->getPlayedMsec() / (double)MEDIA_CTL->getTotalMsec();
	ui.slider->setValue(ui.slider->maximum() * rate);
}

void ControlBar::onSliderToolTip(double pos)
{
	auto time = msecToTime((MEDIA_CTL->getTotalMsec() * pos));
	ui.slider->setTipText(time.toString());
}

void ControlBar::onVideoSeek()
{
	double rate = (double)ui.slider->value() / (double)ui.slider->maximum();
	MEDIA_CTL->seek(rate);
	_slider_timer->start(40);
}

void ControlBar::onPlayOrPause()
{
	if (!ui.btnPlayOrPause->isEnabled()) return;
	if (ui.btnPlayOrPause->text() == 播放)
	{
		MEDIA_CTL->resume();
		ui.btnPlayOrPause->setTipText("暂停");
		ui.btnPlayOrPause->setText(QChar(0xf04c));
	}
	else if (ui.btnPlayOrPause->text() == 暂停)
	{
		MEDIA_CTL->suspend();
		ui.btnPlayOrPause->setTipText("播放");
		ui.btnPlayOrPause->setText(QChar(0xf04b));
	}
}

void ControlBar::onStopPlay()
{

}

void ControlBar::onPrevMedia()
{
	MediaController::OpenPara para = { "D:/Doc/视频文件/林俊杰-不为谁而作的歌(蓝光).mp4", MediaController::FILE, 1 };
	auto w = (MainPlayer*)(AppManager::Get().getWin("MainPlayer"));
	w->openMediaStream(para);
}

void ControlBar::onNextMedia()
{
	static int index = 0;
	if (index == 0)
	{
		MediaController::OpenPara para = { "D:/Doc/视频文件/v1080.mp4", MediaController::FILE, 1 };
		auto w = (MainPlayer*)(AppManager::Get().getWin("MainPlayer"));
		w->openMediaStream(para);
		index = 1;
	}
	else if (index == 1)
	{
		MediaController::OpenPara para = { "D:/Doc/视频文件/那些年.mp4", MediaController::FILE, 1 };
		auto w = (MainPlayer*)(AppManager::Get().getWin("MainPlayer"));
		w->openMediaStream(para);
		index = 0;
	}
}

void ControlBar::onShowSpeed()
{
}

void ControlBar::onShowVolume()
{

}

void ControlBar::onShowSetting()
{

}

void ControlBar::onShowPlayList()
{
	//auto attr = AppManager::Get().getWinAttribute("MainPlayer", "geometry").toRect();
	auto playList = AppManager::Get().getWin("PlayListWidget");
	auto player = AppManager::Get().getWin("MainPlayer");
	if (!playList || !player) return;

	if (playList->isVisible())
	{
		playList->close();
	}
	else
	{
		QRect r = player->geometry();
		playList->show(); //程序启动以后第一次show会卡一下，不知道为什么
		playList->setGeometry(
			r.x() + r.width() - playList->shadowWidth() - player->shadowWidth(),
			r.y(), 200, r.height());
		//if (parentWidget()) parentWidget()->activateWindow();
	}
}
