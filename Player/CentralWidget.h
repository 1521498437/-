/**
* @file CentralWidget.h
* @brief 中心部件
* @authors yandaoyang
* @date 2021/01/03
* @note
*/

#pragma once
#include <QWidget>
#include <QTimer>
#include "ui_CentralWidget.h"

class CentralWidget : public QWidget
{
	Q_OBJECT

public:
	explicit CentralWidget(QWidget* parent = Q_NULLPTR);
	~CentralWidget();
	void init();

signals:
	void sigReplay();
	void sigPlayOrPause();
	void sigShowMaxOrRestore();

private slots:
	void onReadyPlay();
	void onPlayEnd();

protected:
	void mouseReleaseEvent(QMouseEvent*) Q_DECL_OVERRIDE;
	void mouseDoubleClickEvent(QMouseEvent*) Q_DECL_OVERRIDE;

private:
	Ui::CentralWidget ui;
	int _releaseNum{ 0 };
	QTimer* _timer{ nullptr };
};
