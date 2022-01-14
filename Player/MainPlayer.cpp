#include "MainPlayer.h"
#include "MediaController.h"
#include "QAudioPlay.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QKeyEvent>
#include <QAction>
#include <QDebug>
#include <QtWidgets/QApplication>
#include <QDesktopWidget>
//#include <future>
#include "QPainterWnd.h"
//#include <ctime>
#include "Util.h"
#include "AppManager.h"
#pragma execution_character_set("utf-8")

//static int current_msec = 0;
//static int last_moved_moment = 0;
//static AppManager::WinRegister<MainPlayer> g_register_win_1;

AUTO_REGISTER_WINDOW(MainPlayer);

MainPlayer::MainPlayer(QWidget* parent)
    : CommonWindow(parent),
    _device_menu(/*new QMenu(this)*/_base->titleBar()->getDeviceMenu()),
    _videoWnd(new QPainterWnd(_base))
{
   ui.setupUi(this);

    /*_auto_hide_timer->setSingleShot(true);
    //_auto_hide_timer->start(3000);
    connect(_auto_hide_timer, &QTimer::timeout, [this] { 
       // if(current_msec - last_moved_moment > 100)
            ui.ctrlBar->disappear(); 
    });*/
    /*//计时器
    static QTimer* timer = new QTimer;
    connect(timer, &QTimer::timeout, [] {
        current_msec = (current_msec + 1) % INT32_MAX;
    });
    timer->start(1);*/
}

MainPlayer::~MainPlayer()
{
    MEDIA_CTL->stop();
}

void MainPlayer::init()
{
    setMouseTracking(true); //所有子部件都要设置这个属性才能响应无点击的mouseMoveEvent
    setStyleSheet("background:transparent;color:white;");
    _videoWnd->lower();
    _videoWnd->move(_base->shadowWidth(), _base->shadowWidth());
    /*if (Util::loadLiveChannel(QDir::homePath() + "/1.json", ui.btnOpenLive)) //路径缺陷
    {
        qDebug() << "直播频道加载成功";
        auto actions = ui.btnOpenLive->actions();
        for (int i = 0, size = actions.size(); i < size; i++)
            connect(actions.at(i), &QAction::triggered, this, &MainPlayer::onOpenLive);
    }
    //ui.btnOpenCamera->setMenu(_device_menu);*/
    bindSlots();
    
    ui.ctrlBar->init();
    ui.centralWid->init();
}

void MainPlayer::bindSlots()
{
   connect(_base->titleBar(), &TitleBar::sigOpenFile, this, &MainPlayer::onOpenFile);
   //connect(ui.btnOpenLive, &QToolButton::clicked, ui.btnOpenLive, &QToolButton::showMenu); //缺陷
   //connect(ui.btnOpenCamera, &QToolButton::clicked, this, &MainPlayer::onShowDeviceList);
   connect(_base->titleBar(), &TitleBar::sigOpenCamera, this, &MainPlayer::onShowDeviceList);
   connect(_base->titleBar(), &TitleBar::sigRtmpStreaming, this, &MainPlayer::onRtmpStreaming);

    connect(ui.centralWid, &CentralWidget::sigReplay, this, &MainPlayer::onOpenFile);
    connect(ui.centralWid, &CentralWidget::sigShowMaxOrRestore, titleBar(), &TitleBar::onBtnMaxClicked);
    connect(ui.centralWid, &CentralWidget::sigPlayOrPause, ui.ctrlBar, &ControlBar::reqPlayOrPause);
    connect(MEDIA_CTL, &MediaController::sigPlayEnd, this, &MainPlayer::onPlayEnd, Qt::QueuedConnection);
    connect(_base, &BaseWindow::sigShowMax, this, &MainPlayer::onShowMax);
    connect(_base, &BaseWindow::sigRestore, this, &MainPlayer::onRestore);

    //rtmp推流路径错误
   //connect(MEDIA_CTL, &MediaController::sigRtmpError, this, &MainPlayer::onRtmpError, Qt::QueuedConnection);
}

void MainPlayer::onShowMax()
{
    _videoWnd->setGeometry(0, 0, _base->width(), _base->height());
}

void MainPlayer::onRestore()
{
    _videoWnd->setGeometry(
        _base->shadowWidth(), _base->shadowWidth(),
        _base->width() - 2 * _base->shadowWidth(),
        _base->height() - 2 * _base->shadowWidth()
    );
}

void MainPlayer::onOpenFile()
{
   QString filename = QString();
   if (sender() == _base->titleBar())
   {
      static QString path = "D:/Doc/video";//QCoreApplication::applicationDirPath();
      filename = QFileDialog::getOpenFileName(
         this, "请选择视频文件", path,
         tr("视频文件(*.mp4 *.avi *.mov *.flv *.wmv *.mpg *.mpeg *.mkv)")
      );
      if (filename.isEmpty() || filename.isNull())
         return;
      path = QFileInfo(filename).absolutePath();
   }
   else if (sender() == ui.centralWid)
   {
      filename = MEDIA_CTL->last_url;
   }
   MediaController::OpenPara para = { filename, MediaController::FILE, 1 };
   openMediaStream(para);
}

/*void MainPlayer::onOpenLive()
{
    QAction* action = qobject_cast<QAction*>(sender());
    MediaController::OpenPara para = { action->data().toString(), MediaController::LIVE, 3 };
    openMediaStream(para);
    //std::thread(&MainPlayer::openMediaStream, this, para).detach();
}*/

void MainPlayer::onOpenCamera()
{
   QAction* action = qobject_cast<QAction*>(sender());
   MediaController::OpenPara para = { "video=" + action->text(), MediaController::DEVICE, 1 };
   openMediaStream(para);
}

void MainPlayer::onPlayEnd()
{
   /* 不知道为什么当本地视频文件正在播放的时候点击关闭程序，av_read_frame返回的也是AVERROR_EOF，（如果是摄像头则不会这样，直播流还没试）
   目前猜想可能的原因应该是点击关闭的时候调用了avformat_close_input从而demux线程的av_read_frame返回了AVERROR_EOF。
   为了解决点击关闭弹出【视频播放结束】提示框的bug，目前的解决方案只能是设置_app_exit标志位。*/
   //if (_app_exit) return;
   ui.ctrlBar->appear();
   //_base->titleBar()->appear();
   //QMessageBox::information(this, "提示", "视频播放结束！");
}

void MainPlayer::onShowDeviceList()
{
   _device_menu->clear();
   QStringList _list = MEDIA_CTL->getDeviceList();
   QFont font;
   font.setBold(true);
   font.setPointSize(9);
   font.setFamily("Comic Sans MS");
   for (int i = 0; i < _list.size(); i++)
   {
      QAction* a = new QAction(_list.at(i), this);
      a->setFont(font);
      connect(a, &QAction::triggered, this, &MainPlayer::onOpenCamera);
      _device_menu->addAction(a);
   }
   //ui.btnOpenCamera->showMenu();
}

void MainPlayer::onRtmpStreaming()
{
   if (MEDIA_CTL->curPlayingType() != MediaController::DEVICE) //后期考虑加入屏幕共享
   {
      QMessageBox::warning(this, "", "请先打开摄像头");
      return;
   }
   AppManager::Get().showWin("StreamingWidget");
}

//void MainPlayer::onRtmpError()
//{
//   QMessageBox::warning(this, "", "rtmp地址连接错误，推流失败！");
//}

bool MainPlayer::openMediaStream(const std::any& value)
{
   auto para = std::any_cast<MediaController::OpenPara>(value);  //需要判断bad_any_cast
   if (!MEDIA_CTL->open(para, _videoWnd/*ui.videoWnd*/, AUDIO_PLAY))
   {
      QMessageBox::warning(this, "提示", "视频文件打开失败！");
      return false;
   }
   else
   {
      ui.ctrlBar->disappear();
      //_base->titleBar()->disappear();
      return true;
   }
}

void MainPlayer::handleNotify(const QString& sender, const QVariant& data)
{
   if (sender.compare("PlayListWidget") == 0)
   {
      QString url = data.toString();
      MediaController::OpenPara para = { url, MediaController::NODEV, 3 };
      openMediaStream(para);
   }
}

void MainPlayer::closeEvent(QCloseEvent*)
{
    //_app_exit = true;
    //MEDIA_CTL->stop();
    AppManager::Get().closeApp();
}

void MainPlayer::resizeEvent(QResizeEvent*)
{
    _videoWnd->resize(
        _base->width() - 2 * _base->shadowWidth(),
        _base->height() - 2 * _base->shadowWidth()
    );
}

void MainPlayer::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)
    {
        //emit ui.btnPlayOrPause->clicked();
        return;
    }
    QWidget::keyPressEvent(e);
}

void MainPlayer::mouseMoveEvent(QMouseEvent* e)
{
    /*if (ui.ctrlBar->appear())
    {
        _auto_hide_timer->singleShot(3000, [this] {
            if(ui.ctrlBar->visible())
                ui.ctrlBar->disapper();
        });
    }*/
    /*///////////////////////////////
    if (ui.ctrlBar->appear())
    {
        //if(_auto_hide_timer->isActive())
            //_auto_hide_timer->stop();
        //_auto_hide_timer->start(3000);
    }
    _auto_hide_timer->stop();
    _auto_hide_timer->start(3000);
    //last_moved_moment = current_msec;
    ///////////////////////////////*/
    ui.ctrlBar->appear();
   // _base->titleBar()->appear();
    ui.ctrlBar->setGlobalPos(mapToGlobal(ui.ctrlBar->pos()));
    //_base->titleBar()->setGlobalPos(mapToGlobal(_base->titleBar()->pos()));
    ui.ctrlBar->autoDisappear(3000);
    //_base->titleBar()->autoDisappear(3000);
}
