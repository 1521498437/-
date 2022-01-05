#include "AppManager.h"
#include "MainPlayer.h"
#include "PlayListWidget.h"
#include "StreamingWidget.h"
#include "LoginDialog.h"
#include "RegistDialog.h"
#include "LiveSocket.h"
#include "MessageWidget.h"
#include <QFontDatabase>
#include <QtWidgets/QApplication>
#include <QDesktopWidget>

int main(int argc, char* argv[])
{
   QApplication a(argc, argv);

	// 加载字库，用于作为UI图标
	QFontDatabase::addApplicationFont(":/App/Res/fontawesome-webfont.ttf");

	// 连接远程服务器
	LiveSocket::Get().connectToServer("192.168.239.180", 6666);
	
	MainPlayer w1;
	PlayListWidget w2;
	StreamingWidget w3;
	MessageWidget w4;
	APP_MGR->registerWin(&w1);
	APP_MGR->registerWin(&w2);
	APP_MGR->registerWin(&w3);
	APP_MGR->registerWin(&w4);
	w1.init();
	w2.init();
	w3.init();
	w4.init();

	w1.resize(800, 620);
	w1.show();
	//w4.show();
	w1.move((qApp->desktop()->width() - w1.width()) / 2,
		(qApp->desktop()->height() - w1.height()) / 2);

	return a.exec();
}
