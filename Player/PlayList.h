/**
* @file PlayList.h
* @brief 播放列表控件
* @authors yandaoyang
* @date 2021/01/03
* @note 
*/

#pragma once
#include <QListWidget>
#include <QAction>
#include <QMenu>

class PlayList : public QListWidget
{
	Q_OBJECT

public:
	explicit PlayList(QWidget* parent = Q_NULLPTR);
	~PlayList();

signals:
	void sigAddFile(QString filename);

private:
	void addFile();
	void removeFile();

protected:
	void contextMenuEvent(QContextMenuEvent*) Q_DECL_OVERRIDE;

private:
	QMenu*   _menu      { nullptr };
	QAction* _act_add   { nullptr };
	QAction* _act_remove{ nullptr };
	QAction* _act_clear { nullptr };
};
