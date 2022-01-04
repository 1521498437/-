/**
* @file BaseWindow.h
* @brief 基础独立窗口
* @authors yandaoyang
* @date 2020/12/29
* @note 含自定义标题栏、窗口阴影、最小化、
*		最大化、还原、关闭、窗口拖拽、缩放
*/

#pragma once
#include <QWidget>
#include "ui_BaseWindow.h"

struct BaseWindowPrivate;
class BaseWindow : public QWidget
{
	Q_OBJECT

public:
	explicit BaseWindow(QWidget* content);
	~BaseWindow();

public:
	void setResizable(bool);
	int shadowWidth() const;
	TitleBar* titleBar() const;

signals:
	void sigShowMax();
	void sigRestore();
	void sigClose();

protected:
	bool eventFilter(QObject* obj, QEvent* event) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
;	//bool nativeEvent(const QByteArray&, void*, long*) Q_DECL_OVERRIDE;
	void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;

protected slots:
	void onShowMax();
	void onShowNormal();

private:
	Ui::BaseWindow ui;
	BaseWindowPrivate* p{ nullptr };
};
