/**
* @file TitleBar.h
* @brief �Զ��������
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

public: //�Ȳ�����
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
	void onBtnMaxClicked(); //BaseWidgetҲ�����

protected:
	void mouseDoubleClickEvent(QMouseEvent*) Q_DECL_OVERRIDE;

private:
	Ui::TitleBar ui;
	QMenu* _menu{ nullptr };
	QMenu* _device_menu{ nullptr };
	bool    _isMax{ false };
	bool    _maximizable{ true };
	QPoint _globalPos; //����λ�ñ����ɸ�������ָ��������Լ�����mapToGlobal(pos())�����ƫ��
};
