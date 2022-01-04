#include "BubbleTip.h"
#include <QPainter>

BubbleTip::BubbleTip(QWidget* parent)
    : QWidget(parent), _label(new QLabel(this))
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    _label->setAlignment(Qt::AlignCenter);
    _label->setStyleSheet(
        "color:black; "
        "font-weight:bold;" 
        "font-family:youyuan;"
        "padding-bottom:10px;");
    _pixmap.load(":/App/Res/icon/bubble.png");
}

BubbleTip::~BubbleTip()
{

}

void BubbleTip::init(const QString& text)
{
    if (!text.isEmpty())
    {
        _label->setText(text);
        QFontMetrics fm(QFont("youyuan"));
        _pixmap = _pixmap.scaled(
            fm.horizontalAdvance(text) + 10, fm.height() + 16,
            Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        resize(_pixmap.size());
        _label->setGeometry(geometry());
    }
}

void BubbleTip::setText(const QString& text)
{
    _label->setText(text);
}

QString BubbleTip::text() const
{
    return _label->text();
}

void BubbleTip::popup()
{
    if (!_label->text().isEmpty())
    {
        show();
        move(QCursor::pos().x() - width() / 2, QCursor::pos().y() - height());
    }
}

void BubbleTip::paintEvent(QPaintEvent* e)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, _pixmap);
    //QWidget::paintEvent(e);
}

