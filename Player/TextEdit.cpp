#include "TextEdit.h"
#include <QPainter>

TextEdit::TextEdit(QWidget* parent)
	: QTextEdit(parent)
{

}

TextEdit::~TextEdit()
{
}

void TextEdit::paintEvent(QPaintEvent* e)
{
	QPainter painter(this->viewport());
	painter.fillRect(QRect(0, 0, width(), height()), QBrush(QColor(35, 35, 41)));
	QTextEdit::paintEvent(e);
}
