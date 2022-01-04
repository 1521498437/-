/**
* @file CommonWindow.h
* @brief ͨ�ô�����
* @authors yandaoyang
* @date 2020/12/29
* @note �����¶������ڶ�Ӧ�̳д���
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
