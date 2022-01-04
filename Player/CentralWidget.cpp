#include "CentralWidget.h"
#include "Util.h"
#include "MediaController.h"
#include <QDebug>
#include <QTime>

CentralWidget::CentralWidget(QWidget* parent)
	: QWidget(parent), _timer(new QTimer(this))
{
	ui.setupUi(this);
	//����QWidget���и��������޷��ڹ�����setStyleSheet�� �����ڸ������н���
	//setStyleSheet(Util::loadQssFile(":/App/Res/qss/CentralWidget.css"));

	//���¶�ʱ����Ϊ�����ֵ�����˫���¼����ǵ����¼���˫���¼���Ӧ�ô��������¼���
	/*_timer->setSingleShot(true);
	_timer->setInterval(230);
	connect(_timer, &QTimer::timeout, [this] {
		if (_releaseNum == 2)
			emit sigShowMaxOrRestore();
		else if(_releaseNum == 1)
			emit sigPlayOrPause();
		_releaseNum = 0;
	});*/
}

CentralWidget::~CentralWidget()
{
}

void CentralWidget::init()
{
	setStyleSheet(Util::loadQssFile(":/App/Res/qss/CentralWidget.css"));
	ui.btnReplay->setIcon(QIcon(":/App/Res/icon/refresh.png"));
	ui.btnReplay->setMask(QRegion(QRect(ui.btnReplay->pos(), ui.btnReplay->minimumSize()), QRegion::Ellipse));
	ui.btnReplay->hide();
	connect(ui.btnReplay, &QPushButton::clicked, this, &CentralWidget::sigReplay);
	connect(MEDIA_CTL, &MediaController::sigReadyPlay, this, &CentralWidget::onReadyPlay);
	connect(MEDIA_CTL, &MediaController::sigPlayEnd, this, &CentralWidget::onPlayEnd);
}

void CentralWidget::onReadyPlay()
{
	ui.btnReplay->hide();
}

void CentralWidget::onPlayEnd()
{
	ui.btnReplay->show();
}

void CentralWidget::mouseReleaseEvent(QMouseEvent*)
{
	/*_releaseNum += 1;
	_timer->stop();
	_timer->start();*/
}

void CentralWidget::mouseDoubleClickEvent(QMouseEvent*)
{
	//emit sigShowMaxOrRestore();
	emit sigPlayOrPause();
}
