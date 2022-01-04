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
	printf("ֱ��Ƶ������ʧ�ܣ������Ӧ�����ļ�.\n");
	return false;
}

ENTRY
{
	/*// �Զ�д��ʽ����Ŀ¼�µ�1.json�ļ��������ļ�����������Զ�����
	QFile file(QDir::homePath() + "/1.json");
	if (!file.open(QIODevice::ReadWrite)) {
		qDebug() << "File open error";
	}
	else {
		qDebug() << "File open!";
	}
	// ʹ��QJsonObject��������ֵ�ԡ�
	QJsonObject jsonObject;
	jsonObject.insert("name", "tom");
	jsonObject.insert("age", "18");
	jsonObject.insert("time", QDateTime::currentDateTime().toString());

	QJsonArray jsonAry;
	jsonAry.append(jsonObject);
	jsonAry.append(jsonObject);
	jsonAry.append(jsonObject);


	// ʹ��QJsonDocument���ø�json����
	QJsonDocument jsonDoc;
	jsonDoc.setArray(jsonAry);

	// ��json���ı���ʽд���ļ����ر��ļ���
	file.write(jsonDoc.toJson());
	file.close();*/
	if (loadLiveChannel(nullptr))
	{
		printf("ֱ��Ƶ�����سɹ�.\n");
	}
	return 0;
}

