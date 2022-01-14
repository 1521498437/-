#include "WinUtil.h"
#include "CommonWindow.h"
#include <QApplication>
#include <QDesktopWidget>

void WinUtil::MoveToDesktopCenter(CommonWindow* win, int w, int h) {
	if (!win) return;
	win->setVisible(true);
	if (w > 0 && h > 0) 
		win->resize(w, h);
	auto desktop = qApp->desktop();
	auto x = (desktop->width()  - win->width())  * 0.5f;
	auto y = (desktop->height() - win->height()) * 0.5f;
	win->move(x, y);
}
