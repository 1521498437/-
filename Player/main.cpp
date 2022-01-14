#include "AppManager.h"
#include "LiveSocket.h"
#include <QFontDatabase>
#include <QApplication>
#include "WinUtil.h"

int main(int argc, char** argv)
{
   QApplication app(argc, argv);

	// 加载字库，用于作为UI图标
	QFontDatabase::addApplicationFont(":/App/Res/fontawesome-webfont.ttf");

	// 连接远程服务器
	LiveSocket::Get().connectToServer("192.168.239.180", 6666);
	
	/*MainPlayer      w1;
	PlayListWidget  w2;
	StreamingWidget w3;
	MessageWidget   w4;
	AppManager::Get().registerWin(&w1);
	AppManager::Get().registerWin(&w2);
	AppManager::Get().registerWin(&w3);
	AppManager::Get().registerWin(&w4);
	w1.init();
	w2.init();
	w3.init();
	w4.init();*/
	AppManager::Get().createWindows();
	auto mainW = AppManager::Get().getWin("MainPlayer");

	//w4.show();
	WinUtil::MoveToDesktopCenter(mainW, 800, 620);

	return app.exec();
}
