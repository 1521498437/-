#include "StreamingWidget.h"
#include "MatFilterNet.h"
#include <QDebug>
#include <QMessageBox>
#include "MediaController.h"
#pragma execution_character_set("utf-8")
#include "LoginDialog.h"
#include "LiveSocket.h"
#include "Service.h"
#include "AppManager.h"

AUTO_REGISTER_WINDOW(StreamingWidget);

StreamingWidget::StreamingWidget(QWidget *parent)
	: CommonWindow(parent)
{
	ui.setupUi(this);
	setResizable(false);
	//setMinimizable(false);
	setMaximizable(false);
	_base->titleBar()->setMainMenuVisible(false);
	_base->setFixedSize(435, 285);//不加这个标题栏就太大了
	//qDebug() << ui.btnSmooth;


	MatFilterNet::Get()->modFilter(FilterType::kSMOOTH, 0.);
	//MatFilterNet::Get()->modFilter(FilterType::kWHITE, 0.);
	//MatFilterNet::Get()->modFilter(FilterType::kTHIN, 0.);
	//MatFilterNet::Get()->modFilter(FilterType::kBRIGHTNESS, 0.);
	//MatFilterNet::Get()->modFilter(FilterType::kCONTRAST, 0.);
	//MatFilterNet::Get()->modFilter(FilterType::KSATURATION, 0.);
	ui.btnWhite->setDisabled(true);
	ui.labWhite->setDisabled(true);
	ui.btnThin->setDisabled(true);
	ui.labThin->setDisabled(true);
	ui.btnSaturation->setDisabled(true);
	ui.labSaturation->setDisabled(true);
}

StreamingWidget::~StreamingWidget()
{
}

void StreamingWidget::init()
{
	//按钮组初始化
	mBtnGroup = {
		{ui.btnSmooth, {ui.labSmooth, 0}},
		{ui.btnWhite, {ui.labWhite, 0}},
		{ui.btnThin, {ui.labThin, 0}},
		{ui.btnBrightness, {ui.labBrightness, 0}},
		{ui.btnContrast, {ui.labContrast, 0}},
		{ui.btnSaturation, {ui.labSaturation, 0}}
	};//有用到sender()的函数就不要是lambda
	connect(ui.btnSmooth, &QPushButton::clicked, this, &StreamingWidget::onBeautyFaceButtonClicked);
	connect(ui.btnWhite, &QPushButton::clicked, this, &StreamingWidget::onBeautyFaceButtonClicked);
	connect(ui.btnThin, &QPushButton::clicked, this, &StreamingWidget::onBeautyFaceButtonClicked);
	connect(ui.btnBrightness, &QPushButton::clicked, this, &StreamingWidget::onBeautyFaceButtonClicked);
	connect(ui.btnContrast, &QPushButton::clicked, this, &StreamingWidget::onBeautyFaceButtonClicked);
	connect(ui.btnSaturation, &QPushButton::clicked, this, &StreamingWidget::onBeautyFaceButtonClicked);
	emit ui.btnSmooth->clicked();//一开始在磨皮上

	//刻度条与控制按钮初始化
	connect(ui.slider, &Slider::sigPressed, this, &StreamingWidget::onBeautyFaceSliderChanged);
	connect(ui.slider, &Slider::sliderMoved, this, &StreamingWidget::onBeautyFaceSliderChanged);
	connect(ui.btnCtrl, &QPushButton::clicked, this, &StreamingWidget::onBtnCtrlClicked);

	//直播业务逻辑
	connect(&LiveSocket::Get(), &LiveSocket::startLiveRes, [this](QByteArray arr) {
		auto res = (srv::StartLiveRes*)arr.data();
		if (res->success)
		{
			MEDIA_CTL->startStreaming(ui.edtRtmp->text());
			AppManager::Get().sendNotify(metaObject()->className(), "MessageWidget", LoginDialog::UserName());
			ui.btnCtrl->setText("开始直播");
			//LiveSocket::Get().watchLiveReq("yandaoyang", "佚名");
		}
		else
			QMessageBox::warning(nullptr, "", "此直播路径已被占用！");
	});
}

void StreamingWidget::onBeautyFaceButtonClicked()
{
	auto clickedBtn = (QPushButton*)sender();
	if (mCurBtn == clickedBtn) return;
	if (mCurBtn)
	{
		mCurBtn->setStyleSheet("border:none;");
		mBtnGroup[mCurBtn].first->setStyleSheet("color:white;");
	}
	clickedBtn->setStyleSheet("border:2px solid rgb(255, 0, 0);");
	mBtnGroup[clickedBtn].first->setStyleSheet("color:red;");
	ui.slider->setValue(mBtnGroup[clickedBtn].second);
	mCurBtn = clickedBtn;
}

void StreamingWidget::onBeautyFaceSliderChanged()
{
	if (!mCurBtn) return;
	int value = ui.slider->value();
	mBtnGroup[mCurBtn].second = value;
	if (mCurBtn == ui.btnSmooth)
		MatFilterNet::Get()->modFilter(FilterType::kSMOOTH, (double)value);
	//else if (mCurBtn == ui.btnWhite)
		//MatFilterNet::Get()->modFilter(FilterType::kWHITE, (double)value);
	//else if (mCurBtn == ui.btnThin)
		//MatFilterNet::Get()->modFilter(FilterType::kTHIN, (double)value);
	else if (mCurBtn == ui.btnBrightness)
		MatFilterNet::Get()->modFilter(FilterType::kBRIGHTNESS, (double)value);
	else if (mCurBtn == ui.btnContrast)
		MatFilterNet::Get()->modFilter(FilterType::kCONTRAST, (double)value);
	//else if (mCurBtn == ui.btnSaturation)
		//MatFilterNet::Get()->modFilter(FilterType::KSATURATION, (double)value);
}

void StreamingWidget::onBtnCtrlClicked()
{
	if (LoginDialog::UserName() == QString())
	{
		LoginDialog::Get().exec();
		return;
	}
	if(ui.edtRtmp->text().isEmpty())
	{
		QMessageBox::warning(this, "", "rtmp推流地址不能为空！");
		return;
	}
	LiveSocket::Get().startLiveReq(LoginDialog::UserName(), ui.edtRtmp->text()); //缺陷

	//MEDIA_CTL->startStreaming(ui.edtRtmp->text());
}
