#include "MessageWidget.h"
#include "LiveSocket.h"
#include "Service.h"
#include "LoginDialog.h"
#include <QDateTime>
#include "AppManager.h"
#pragma execution_character_set("utf-8")

MessageWidget::MessageWidget(QWidget* parent)
	: CommonWindow(parent)
{
	ui.setupUi(this);
	setResizable(false);
	//setMinimizable(false);
	setMaximizable(false);
	_base->titleBar()->setMainMenuVisible(false);
}

MessageWidget::~MessageWidget()
{

}

void MessageWidget::handleNotify(const QString& sender, const QVariant& data)
{
	if (sender.compare("StreamingWidget") == 0)
	{
		QFont font;
		font.setBold(true);
		font.setPointSize(9);
		font.setFamily("youyuan");
		QListWidgetItem* item = new QListWidgetItem(data.toString(), ui.listWidget);
		item->setFont(font);
		_onlineCount += 1;
		ui.btnOnline->setText(QString("在线 %1").arg(_onlineCount));

		auto player = APP_MGR->getWin("MainPlayer");
		if (!player) return;
		QRect r = player->geometry();
		this->show();
		this->setGeometry(r.x() - 240 + this->shadowWidth() + player->shadowWidth(), r.y(), 240, r.height());
	}
	else if (sender.compare("PlayListWidget") == 0)
	{
		auto player = APP_MGR->getWin("MainPlayer");
		if (!player) return;
		QRect r = player->geometry();
		this->show();
		this->setGeometry(r.x() + r.width() - this->shadowWidth() - player->shadowWidth(), r.y(), 200, r.height());
	}
}

bool MessageWidget::init()
{
	connect(ui.btnMsg, &QPushButton::clicked, [this] {ui.stackedWidget->setCurrentWidget(ui.msgPage); });
	connect(ui.btnOnline, &QPushButton::clicked, [this] {ui.stackedWidget->setCurrentWidget(ui.onlinePage); });

	connect(&LiveSocket::Get(), &LiveSocket::roomInfoRec, [this](QByteArray arr, int count) {

		auto recs = (srv::RoomInfoRec*)arr.data();

		QFont font;
		font.setBold(true);
		font.setPointSize(9);
		font.setFamily("youyuan");
		for (int i = 0; i < count; ++i)
		{
			QListWidgetItem* item = new QListWidgetItem(recs[i].online_name, ui.listWidget);
			item->setFont(font);
		}
		_onlineCount += count;
		ui.btnOnline->setText(QString("在线 %1").arg(_onlineCount));
	});

	connect(ui.btnSend, &QPushButton::clicked, [this] {

		if (ui.lineEdit->text().isEmpty()) return;

		//LiveSocket::Get().watchLiveReq("yandaoyang", "caiwenxian");
		selfSpeak(ui.lineEdit->text());
		LiveSocket::Get().message(LoginDialog::UserName(), ui.lineEdit->text());
		//LiveSocket::Get().message("佚名", ui.lineEdit->text());
		ui.lineEdit->clear();
	});

	connect(&LiveSocket::Get(), static_cast<void(LiveSocket::*)(QByteArray)>(&LiveSocket::message), [this](QByteArray arr) {
		auto msg = (srv::MessageReq*)arr.data();
		otherSpeak(msg->user, msg->msg);
	});

	return true;
}

void MessageWidget::selfSpeak(const QString& str)
{
	QString msg = QString(
		"<p align = \"right\" style = \"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'youyuan';\" > <br / > < / p>"
		"<p align = \"right\" style = \" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; "
		"-qt-block-indent:0; text-indent:0px;\">"
		"<span style = \" font-family:'SimSun'; font-weight:600;\">[我 %1]：< / span>< / p>"
		"<p align = \"right\" style = \" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;"
		"-qt-block-indent:0; text-indent:0px;\">"
		"<span style = \" font-family:'youyuan';\">%2< / span>< / p>< / body>< / html>"
	).arg(QDateTime::currentDateTime().toString("hh:mm:ss")).arg(str);
	ui.textEdit->append(msg);
}

void MessageWidget::otherSpeak(const QString& other, const QString& str)
{
	QString msg = QString(
		"<p style = \"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'youyuan';\" > <br / > < / p>"
		"<p style = \" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; "
		"-qt-block-indent:0; text-indent:0px;\">"
		"<span style = \" font-family:'SimSun'; font-weight:600;\">[%1 %2]：< / span>< / p>"
		"<p style = \" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;"
		"-qt-block-indent:0; text-indent:0px;\">"
		"<span style = \" font-family:'youyuan';\">%3< / span>< / p>< / body>< / html>"
	).arg(other).arg(QDateTime::currentDateTime().toString("hh:mm:ss")).arg(str);
	ui.textEdit->append(msg);
}
