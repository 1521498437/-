#pragma once
#include <any>
#include <QMenu>
#include <QtWidgets/QWidget>
#include "ui_MainPlayer.h"
#include "CommonWindow.h"

struct OpenPara;
struct QPainterWnd;
class MainPlayer : public CommonWindow
{
	Q_OBJECT

public:
	explicit MainPlayer(QWidget* parent = Q_NULLPTR);
	~MainPlayer();
	void init();
	bool openMediaStream(const std::any&);

private:
	void bindSlots();
	//bool openMediaStream(const std::any&);
	void handleNotify(const QString&, const QVariant&) override;
	//Q_INVOKABLE int test() const { return 999; }

private slots:
	void onShowMax();
	void onRestore();
	void onOpenFile();
	//void onOpenLive();
	void onOpenCamera();
	void onPlayEnd();
	void onShowDeviceList();
	void onRtmpStreaming();
	//void onRtmpError();

protected:
	void closeEvent(QCloseEvent*) Q_DECL_OVERRIDE;
	void keyPressEvent(QKeyEvent*) Q_DECL_OVERRIDE;
	void resizeEvent(QResizeEvent*) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent*) Q_DECL_OVERRIDE;

private:
	Ui::MainPlayer ui;
	//bool    _app_exit    { false };
	QMenu* _device_menu{ nullptr };
	QPainterWnd* _videoWnd{ nullptr };
};
