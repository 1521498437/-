#include "TestDef.h"
#ifdef JSON_DEMO
#define ENTRY int main(int argc, char *argv[])
#else
#define ENTRY static int func(int argc, char *argv[])
#endif

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QString>
#include <QDebug>
#include <QFile>
#include <QDateTime>
#include <QDir>
#include <QMenu>

bool loadLiveChannel(QMenu* menu)
{
	if (!menu) return false;
	QFile file(QDir::homePath() + "/1.json");
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
			QAction* action = new QAction(name.toString(), menu);
			action->setData(url.toString());
			menu->addAction(action);
		}
		if (i < liveAry.size())
			break;
		file.close();
		return true;
	} while (0);

	file.close();
	printf("直播频道加载失败，请检查对应配置文件.\n");
	return false;
}

ENTRY
{
	/*// 以读写方式打开主目录下的1.json文件，若该文件不存在则会自动创建
	QFile file(QDir::homePath() + "/1.json");
	if (!file.open(QIODevice::ReadWrite)) {
		qDebug() << "File open error";
	}
	else {
		qDebug() << "File open!";
	}
	// 使用QJsonObject对象插入键值对。
	QJsonObject jsonObject;
	jsonObject.insert("name", "tom");
	jsonObject.insert("age", "18");
	jsonObject.insert("time", QDateTime::currentDateTime().toString());

	QJsonArray jsonAry;
	jsonAry.append(jsonObject);
	jsonAry.append(jsonObject);
	jsonAry.append(jsonObject);


	// 使用QJsonDocument设置该json对象
	QJsonDocument jsonDoc;
	jsonDoc.setArray(jsonAry);

	// 将json以文本形式写入文件并关闭文件。
	file.write(jsonDoc.toJson());
	file.close();*/
	if (loadLiveChannel(nullptr))
	{
		printf("直播频道加载成功.\n");
	}
	return 0;
}

