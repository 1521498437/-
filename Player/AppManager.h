/**
* @file AppManager.h
* @brief 全局应用管理层
* @authors yandaoyang
* @date 2021/01/03
* @note
*/

#pragma once
#include <Map>
#include <QObject>
#include "CommonWindow.h"
#define APP_MGR AppManager::GetInstance()

class AppManager : public QObject
{
	Q_OBJECT

public:
	static AppManager* GetInstance(); /* 单例接口 */

public: 
	void closeApp();
	void registerWin(CommonWindow*);
	void removeWin(const QString& name);
	void showWin(const QString& name);
	void hideWin(const QString& name);
	CommonWindow* getWin(const QString& name);
	void sendNotify(const QString& sender, const QString& receiver, const QVariant& data);
	//QVariant getWinAttribute(const QString& name, const QString& attrname);
	//QVariant getWinProperty(const QString& name, const QString& property);

private:
	AppManager();
	~AppManager();
	AppManager(AppManager&&) = delete;
	AppManager(const AppManager&) = delete;
	AppManager& operator=(AppManager&&) = delete;
	AppManager& operator=(const AppManager&) = delete;

private:
	//如果传递进来的是在堆区创建的窗体，需要调用者自己管理窗体的生命周期
	QMap<QString, CommonWindow*> _wins;//不允许管理前向声明的对象
};
