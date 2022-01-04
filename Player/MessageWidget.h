#pragma once
#include "CommonWindow.h"
#include "ui_MessageWidget.h"

class MessageWidget : public CommonWindow
{
	Q_OBJECT

public:
	MessageWidget(QWidget* parent = Q_NULLPTR);
	~MessageWidget();
	bool init();

protected:
	void handleNotify(const QString&, const QVariant&) override;
	void selfSpeak(const QString&);
	void otherSpeak(const QString&, const QString&);

private:
	Ui::MessageWidget ui;
	int _onlineCount{ 0 };
};

