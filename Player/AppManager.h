/**
* @file AppManager.h
* @brief ȫ��Ӧ�ù����
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
	static AppManager* GetInstance(); /* �����ӿ� */

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
	//������ݽ��������ڶ��������Ĵ��壬��Ҫ�������Լ����������������
	QMap<QString, CommonWindow*> _wins;//���������ǰ�������Ķ���
};
