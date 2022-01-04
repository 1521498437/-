/**
* @file BubbleTip.h
* @brief ∆¯≈›Ã· æ
* @authors yandaoyang
* @date 2021/01/01
* @note
*/

#pragma once
#include <QLabel>

class BubbleTip : public QWidget
{
	Q_OBJECT

public:
    explicit BubbleTip(QWidget* parent = Q_NULLPTR);
    ~BubbleTip();

public:
    void init(const QString&);
    void setText(const QString&);
    QString text() const;
    void popup();

protected:
    void paintEvent(QPaintEvent*) Q_DECL_OVERRIDE;

private:
    QPixmap _pixmap;
    QLabel* _label{ nullptr };
};
