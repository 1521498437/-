/**
* @file PlayListWidget.h
* @brief 播放列表窗口
* @authors yandaoyang
* @date 2021/01/03
* @note 独立窗口
*/

#pragma once
#include <QMap>
#include "CommonWindow.h"
#include "ui_PlayListWidget.h"

class PlayListWidget : public CommonWindow
{
	Q_OBJECT

public:
	PlayListWidget(QWidget* parent = Q_NULLPTR);
	~PlayListWidget();
	bool init();

private slots:
	void onSelect(const QString&);

private:
	Ui::PlayListWidget ui;
	QMap<QString, QString> _urls;
	QMap<QString, QString> _liveRooms;
};
