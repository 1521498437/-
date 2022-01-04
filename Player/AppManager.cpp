#include "AppManager.h"

AppManager* AppManager::GetInstance()
{
	static AppManager instance;
	return &instance;
}

AppManager::AppManager()
	: QObject(nullptr)
{
}

AppManager::~AppManager()
{
}

void AppManager::closeApp()
{
	for (const auto& w: _wins)
		w->close();
}

void AppManager::registerWin(CommonWindow* cw)
{
	//这里用到了Qt的元对象反射.
	_wins.insert(cw->metaObject()->className(), cw);
}

void AppManager::removeWin(const QString& name)
{
	_wins.remove(name);
}

void AppManager::showWin(const QString& name)
{
	if (_wins.find(name) != _wins.end())
	{
		_wins[name]->show();
	}
}

void AppManager::hideWin(const QString& name)
{
	if (_wins.find(name) != _wins.end())
	{
		_wins[name]->hide();
	}
}

CommonWindow* AppManager::getWin(const QString& name)
{
	if (_wins.find(name) != _wins.end())
	{
		return _wins[name];
	}
	return nullptr;
}

void AppManager::sendNotify(const QString& sender, const QString& receiver, const QVariant& data)
{
	if (_wins.find(receiver) != _wins.end())
	{
		_wins[receiver]->handleNotify(sender, data);
	}
}

/*QVariant AppManager::getWinProperty(const QString& name, const QString& property)
{
	if (_wins.find(name) != _wins.end())
	{
		return _wins[name]->property(property.toStdString().c_str());
	}
	return QVariant();
}

QVariant AppManager::getWinAttribute(const QString& name, const QString& attrname)
{
	//调用的函数必须被标明为Q_INVOKABLE
	if (_wins.find(name) != _wins.end())
	{
		QVariant var;
		return QMetaObject::invokeMethod(
			_wins[name],
			attrname.toStdString().c_str(),
			//"property",
			Qt::DirectConnection,
			Q_RETURN_ARG(QVariant, var)
			//,Q_ARG(const char*, attrname.toStdString().c_str())
		) ? var : QVariant();
	}
	return QVariant();
}*/
