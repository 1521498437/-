#include "AppManager.h"

AppManager& AppManager::Get()
{
	static AppManager instance;
	return instance;
}

AppManager::AppManager()
	//: QObject(nullptr)
{
}

AppManager::~AppManager()
{
	for (auto& win : mWindowMap)
		SAFE_DELETE(win);
	mWindowMap.clear();
	mCreatorList.clear();
}

void AppManager::closeApp()
{
	for (const auto& w: mWindowMap)
		w->close();
}

void AppManager::registerWinCreator(WinCreator creator)
{
	mCreatorList.append(creator);
}

void AppManager::createWindows() 
{
	for (const auto creator : mCreatorList) {
		auto win = std::invoke(creator);
		mWindowMap.insert(win->metaObject()->className(), win);
		win->init();
	}
}

/*void AppManager::registerWin(CommonWindow* cw)
{
	//这里用到了Qt的元对象反射.
	mWindowMap.insert(cw->metaObject()->className(), cw);
}

void AppManager::removeWin(const QString& name)
{
	mWindowMap.remove(name);
}*/

void AppManager::showWin(const QString& name)
{
	if (mWindowMap.find(name) != mWindowMap.end())
	{
		mWindowMap[name]->show();
	}
}

void AppManager::hideWin(const QString& name)
{
	if (mWindowMap.find(name) != mWindowMap.end())
	{
		mWindowMap[name]->hide();
	}
}

CommonWindow* AppManager::getWin(const QString& name)
{
	if (mWindowMap.find(name) != mWindowMap.end())
	{
		return mWindowMap[name];
	}
	return nullptr;
}

void AppManager::sendNotify(const QString& sender, const QString& receiver, const QVariant& data)
{
	if (mWindowMap.find(receiver) != mWindowMap.end())
	{
		mWindowMap[receiver]->handleNotify(sender, data);
	}
}

/*QVariant AppManager::getWinProperty(const QString& name, const QString& property)
{
	if (mWindowMap.find(name) != mWindowMap.end())
	{
		return mWindowMap[name]->property(property.toStdString().c_str());
	}
	return QVariant();
}

QVariant AppManager::getWinAttribute(const QString& name, const QString& attrname)
{
	//调用的函数必须被标明为Q_INVOKABLE
	if (mWindowMap.find(name) != mWindowMap.end())
	{
		QVariant var;
		return QMetaObject::invokeMethod(
			mWindowMap[name],
			attrname.toStdString().c_str(),
			//"property",
			Qt::DirectConnection,
			Q_RETURN_ARG(QVariant, var)
			//,Q_ARG(const char*, attrname.toStdString().c_str())
		) ? var : QVariant();
	}
	return QVariant();
}*/
