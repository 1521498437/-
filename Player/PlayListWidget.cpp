#include "PlayListWidget.h"
#include "Util.h"
#include "AppManager.h"
#include <QDebug>
#include <QDir>
#include <QDesktopWidget>
#include "LoginDialog.h"
#include "LiveSocket.h"
#include "Service.h"
#pragma execution_character_set("utf-8")

PlayListWidget::PlayListWidget(QWidget* parent)
	: CommonWindow(parent)
{
	ui.setupUi(this);
	setResizable(false);
	//setMinimizable(false);
	setMaximizable(false);
}

PlayListWidget::~PlayListWidget()
{
}

bool PlayListWidget::init()
{
	setStyleSheet(Util::loadQssFile(":/App/Res/qss/PlayListWidget.css"));
	_base->titleBar()->setMainMenuVisible(false);
	ui.stackedWidget->setCurrentWidget(ui.tvListPage);

	auto urls = Util::loadTvUrls(QDir::homePath() + "/1.json");//路径缺陷
	if (!urls.empty())
	{
		QFont font;
		font.setBold(true);
		font.setPointSize(9);
		font.setFamily("youyuan");
		for (const auto& [name, url] : urls) {
			QListWidgetItem* item = new QListWidgetItem(name, ui.tvList);
			item->setFont(font);
			_urls.insert(name, url);
		}
	}

	connect(ui.btnTvList, &QPushButton::clicked, [this] {ui.stackedWidget->setCurrentWidget(ui.tvListPage); });
	connect(ui.btnFileList, &QPushButton::clicked, [this] {ui.stackedWidget->setCurrentWidget(ui.fileListPage); });
	//connect(ui.liveList, &QListWidget::itemDoubleClicked, [this](QListWidgetItem* it) {emit sigOpenLive(it->data(0).toString(); });
	connect(ui.tvList, &QListWidget::itemDoubleClicked, [this](QListWidgetItem* it) {
		APP_MGR->sendNotify(metaObject()->className(), "MainPlayer", _urls.value(it->text())); });
	//connect(ui.edtSelect, &QLineEdit::textEdited, this, &PlayListWidget::onSelect);

	connect(ui.liveList, &QListWidget::itemDoubleClicked, [this](QListWidgetItem* it) {
		LiveSocket::Get().watchLiveReq("yandaoyang", LoginDialog::UserName());
		APP_MGR->sendNotify(metaObject()->className(), "MainPlayer", _liveRooms.value(it->text()));
		APP_MGR->sendNotify(metaObject()->className(), "MessageWidget", QVariant());
	});

	connect(ui.btnLiveList, &QPushButton::clicked, [this] {
		if (LoginDialog::UserName() == QString())
		{
			auto* dlg = &LoginDialog::Get();
			dlg->exec();
		}
		if (LoginDialog::UserName() != QString())
			ui.stackedWidget->setCurrentWidget(ui.liveListPage);
	});

	//直播房间业务处理
	connect(&LiveSocket::Get(), &LiveSocket::livingUserRec, [this](QByteArray arr, int count) {
		auto recs = (srv::LivingUserRec*)arr.data();
		QFont font;
		font.setBold(true);
		font.setPointSize(9);
		font.setFamily("youyuan");
		for (int i = 0; i < count; ++i)
		{
			QListWidgetItem* item = new QListWidgetItem(QString("主播:") + recs[i].user, ui.liveList);
			item->setFont(font);
			_liveRooms.insert(item->text(), recs[i].url);
		}
	});

	//2021-04-23
	connect(ui.edtSelect, &QLineEdit::textChanged, [this](const QString& text) {
		QVector<int> tmp;

		for (int i = 0; i < ui.tvList->count(); i++)
		{
			//printf("%s\n", ui.tvList->item(i)->text().toStdString().c_str());
			if (false == ui.tvList->item(i)->text().contains(text))
			{
				tmp.push_back(i);
			}
		}

		for (int i = tmp.size() - 1; i >= 0; i--)
		{
			ui.tvList->takeItem(tmp.at(i));
		}
	});



	return true;
}

void PlayListWidget::onSelect(const QString& str)
{
}
