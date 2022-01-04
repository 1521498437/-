#pragma once
#include <QMap>
#include <QWidget>
#include "CommonWindow.h"
#include "ui_StreamingWidget.h"

class StreamingWidget : public CommonWindow
{
	Q_OBJECT

public:
	StreamingWidget(QWidget *parent = Q_NULLPTR);
	~StreamingWidget();
	void init();

private slots:
	void onBeautyFaceButtonClicked();
	void onBeautyFaceSliderChanged();
	void onBtnCtrlClicked();

private:
	Ui::StreamingWidget ui;
	QPushButton* mCurBtn{ nullptr };
	QMap<QPushButton*, QPair<QLabel*, int>> mBtnGroup;
};
