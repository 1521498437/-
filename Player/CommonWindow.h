/**
* @file CommonWindow.h
* @brief 通用窗口类
* @authors yandaoyang
* @date 2020/12/29
* @note 所有新独立窗口都应继承此类
*/

#pragma once
#include <QWidget>
#include "BaseWindow.h"

class CommonWindow : public QWidget
{
	Q_OBJECT

public:
	explicit CommonWindow(QWidget* parent = Q_NULLPTR);
	virtual ~CommonWindow();

public:
	void show();
	void hide();
	void close();
	void move(int, int);
	void resize(int, int);
	void setResizable(bool);
	void setMinimizable(bool);
	void setMaximizable(bool);
	int shadowWidth() const;
	TitleBar* titleBar() const;
	const QRect& geometry() const;
	void setGeometry(int ax, int ay, int aw, int ah);
	virtual void handleNotify(const QString&, const QVariant&);

protected:
	void enterEvent(QEvent*) Q_DECL_OVERRIDE;

protected:
	BaseWindow* _base{ nullptr };
};
