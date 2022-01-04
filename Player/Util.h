/**
* @file Util.h
* @brief 通用工具类
* @authors yandaoyang
* @date 2020/12/29
* @note
*/

#pragma once
#include <list>
#include <tuple>
#include <QString>
#include <QPushButton>

using Slot = void(QObject::*)();
class Util
{
public:
	static QString loadQssFile(const QString& filename);
	static void setButtonIcon(QPushButton*, QChar icon, int iconSize);
	static std::list<std::tuple<QString, QString>> loadTvUrls(const QString& filename);
	static bool loadLiveChannel(const QString& filename, QAbstractButton*/*QObject* receiver, 
																		 Slot slot,*/);
};

