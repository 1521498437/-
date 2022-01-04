/**
* @file TitleBar.h
* @brief 自定义标题栏
* @authors yandaoyang
* @date 2020/12/28
* @note
*/

#pragma once
#include <QWidget>
#include <QMenu>
#include "ui_TitleBar.h"
#include "IGradualVisible.h"

class TitleBar : public QWidget, public IGradualVisible
{
	Q_OBJECT

public:
	explicit TitleBar(QWidget* parent = Q_NULLPTR);
	~TitleBar();
	void init();

public:
	int buttonsWidth() const;
	void setMinimizable(bool);
	void setMaximizable(bool);
	bool isMax() const { return _isMax; }

public: //迫不得已
	QMenu* getDeviceMenu() { return _device_menu; }
	void setMainMenuVisible(bool b) { ui.btnMainMenu->setVisible(b); }
	void setGlobalPos(const QPoint& p) { _globalPos = p; }
	bool disappear(int msec = 1000) override;

signals:
	void sigShowMin();
	void sigShowMax();
	void sigRestore();
	void sigCloseWin();

	void sigOpenFile();
	void sigOpenCamera();
	void sigRtmpStreaming();

public slots:
	void onBtnMaxClicked(); //BaseWidget也会调用

protected:
	void mouseDoubleClickEvent(QMouseEvent*) Q_DECL_OVERRIDE;

private:
	Ui::TitleBar ui;
	QMenu* _menu{ nullptr };
	QMenu* _device_menu{ nullptr };
	bool    _isMax{ false };
	bool    _maximizable{ true };
	QPoint _globalPos; //绝对位置必须由父窗口来指定，如果自己调用mapToGlobal(pos())会存在偏差
};
