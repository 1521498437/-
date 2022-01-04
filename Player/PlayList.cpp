#include "PlayList.h"
#include <QFileDialog>
#include <QContextMenuEvent>
#pragma execution_character_set("utf-8")

PlayList::PlayList(QWidget* parent)
	: QListWidget(parent),
	_menu(new QMenu(this)),
	_act_add(new QAction(this)),
	_act_remove(new QAction(this)),
	_act_clear(new QAction(this))
{
	_act_add->setText("添加");
	_menu->addAction(_act_add);
	_act_remove->setText("移除所选项");
	QMenu* remove_menu = _menu->addMenu("移除");
	remove_menu->addAction(_act_remove);
	_act_clear->setText("清空列表");
	_menu->addAction(_act_clear);

	connect(_act_add, &QAction::triggered, this, &PlayList::addFile);
	connect(_act_remove, &QAction::triggered, this, &PlayList::removeFile);
	connect(_act_clear, &QAction::triggered, this, &QListWidget::clear);
}

PlayList::~PlayList()
{
}

void PlayList::addFile()
{
	static QString path = "D:/Doc/video";
	QStringList filenames = QFileDialog::getOpenFileNames(this, "请选择视频文件", path,
		tr("视频文件(*.mp4 *.avi *.mov *.flv *.wmv *.mpg *.mpeg *.mkv)"));
	if (filenames.size() == 0) return;
	path = QFileInfo(filenames.at(0)).absolutePath();
	for (const QString& filename : filenames)
	{
		emit sigAddFile(filename);
	}
}

void PlayList::removeFile()
{
	takeItem(currentRow());
}

void PlayList::contextMenuEvent(QContextMenuEvent* e)
{
	_menu->exec(e->globalPos());
}
