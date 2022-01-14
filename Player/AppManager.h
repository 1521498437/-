/**
* @file AppManager.h
* @brief 全局应用管理层
* @authors yandaoyang
* @date 2021/01/03
* @note
*/

#pragma once
#include <QMap>
//#include <QObject>
#include "Macro.h"
#include "CommonWindow.h"

class AppManager final// : public QObject
{
	//Q_OBJECT

public:
	static AppManager& Get();

	template<typename T>
	struct WinRegister {
		explicit WinRegister() {
			AppManager::Get().registerWinCreator([]()->CommonWindow* { return new T; });
		}
	};

public:
	using WinCreator = CommonWindow*(*)();
	void registerWinCreator(WinCreator);
	void createWindows();
	void closeApp();
	// void registerWin(CommonWindow*);
	// void removeWin(const QString& name);
	void showWin(const QString& name);
	void hideWin(const QString& name);
	CommonWindow* getWin(const QString& name);
	void sendNotify(const QString& sender, const QString& receiver, const QVariant& data);
	//QVariant getWinAttribute(const QString& name, const QString& attrname);
	//QVariant getWinProperty(const QString& name, const QString& property);

private:
	AppManager();
	~AppManager();
	DISABLE_COPY_ASSIGN(AppManager)
	DISABLE_MOVE_ASSIGN(AppManager)

private:
	//如果传递进来的是在堆区创建的窗体，需要调用者自己管理窗体的生命周期
	QMap<QString, CommonWindow*> mWindowMap;//不允许管理前向声明的对象
	QList<WinCreator> mCreatorList;
};

#define AUTO_REGISTER_WINDOW(class) \
	static AppManager::WinRegister<class> __g_##class##_register__;