#include "Util.h"
#include <QFile>
#include <QMenu>
#include <QDebug>
#include <QAction>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#pragma execution_character_set("utf-8")

QString Util::loadQssFile(const QString& filename)
{
	QFile qssFile(filename);
	if (qssFile.open(QIODevice::ReadOnly))
	{
		QString qssStr = qssFile.readAll();
		qssFile.close();
		return qssStr;
	}
	qDebug() << qssFile.fileName() << "qss样式文件加载失败";
	return QString();
}

void Util::setButtonIcon(QPushButton* btn, QChar icon, int iconSize)
{
	QFont font;
	font.setFamily("FontAwesome");
	font.setPointSize(iconSize);
	btn->setFont(font);
	btn->setText(icon);
}

std::list<std::tuple<QString, QString>> Util::loadTvUrls(const QString& filename)
{
	std::list<std::tuple<QString, QString>> urls;
	QFile file(filename);
	do {
		if (!file.open(QIODevice::ReadOnly))
			break;
		QJsonParseError error;
		auto doc(QJsonDocument::fromJson(file.readAll(), &error));
		if (doc.isNull() || error.error != QJsonParseError::NoError)
			break;
		if (!doc.isArray())
			break;
		QJsonArray liveAry = doc.array();
		int i = 0;
		for (; i < liveAry.size(); i++)
		{
			if (!liveAry[i].isObject())
				break;
			auto live(liveAry[i].toObject());
			if (!live.contains("name") || !live.contains("url"))
				break;
			auto name = live.value("name"), url = live.value("url");
			if (!name.isString() || !url.isString())
				break;
			urls.emplace_back(std::make_tuple(name.toString(), url.toString()));
		}
		if (i < liveAry.size())
			break;
		file.close();
		return urls;
	} while (0);

	file.close();
	qDebug() << file.fileName() << "直播频道加载失败，请检查对应配置文件.";
	return urls;
}

bool Util::loadLiveChannel(const QString& filename, QAbstractButton* btn/*, QObject* receiver, 
																		Slot slot*/)
{
	if (!btn) return false;
	QFile file(filename);
	do {
		if (!file.open(QIODevice::ReadOnly))
			break;
		QJsonParseError error;
		auto doc(QJsonDocument::fromJson(file.readAll(), &error));
		if (doc.isNull() || error.error != QJsonParseError::NoError)
			break;
		if (!doc.isArray())
			break;
		QJsonArray liveAry = doc.array();
		int i = 0;
		for (; i < liveAry.size(); i++)
		{
			if (!liveAry[i].isObject())
				break;
			auto live(liveAry[i].toObject());
			if (!live.contains("name") || !live.contains("url"))
				break;
			auto name = live.value("name"), url = live.value("url");
			if (!name.isString() || !url.isString())
				break;
			//qDebug() << name.toString() << url.toString();
			QAction* action = new QAction(name.toString(), btn);
			action->setData(url.toString());
			//QObject::connect(action, &QAction::triggered, receiver, slot);
			btn->addAction(action);
		}
		if (i < liveAry.size())
			break;
		file.close();
		return true;
	} while (0);

	file.close();
	qDebug() << file.fileName() << "直播频道加载失败，请检查对应配置文件.";
	return false;
}



