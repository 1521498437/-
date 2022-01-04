/**
* @file ControlBar.h
* @brief ���ſ�����
* @authors yandaoyang
* @date 2020/12/28
* @note 
*/

#pragma once
#include <QWidget>
#include "ui_ControlBar.h"
#include "IGradualVisible.h"

class ControlBar : public QWidget, public IGradualVisible
{
	Q_OBJECT

public:
	explicit ControlBar(QWidget* parent = Q_NULLPTR);
	~ControlBar();
	void init();

public:
	void setGlobalPos(const QPoint& p);
	bool disappear(int msec = 1000) override;

signals:
	void reqPlayOrPause();

private:
	void bindSlots();
	QTime msecToTime(int64_t msec);

private slots:
	void onReadyPlay();
	void onPlayEnd();
	void onMomentUpdate();
	void onSliderUpdate();
	void onSliderToolTip(double pos);
	void onVideoSeek();
	void onPlayOrPause();
	void onStopPlay();
	void onPrevMedia();
	void onNextMedia();
	void onShowSpeed();
	void onShowVolume();
	void onShowSetting();
	void onShowPlayList();

private:
	Ui::ControlBar ui;
	QPoint  _globalPos; //����λ�ñ����ɸ�������ָ��������Լ�����mapToGlobal(pos())�����ƫ��
	QTimer* _moment_timer{ nullptr };
	QTimer* _slider_timer{ nullptr };
};
