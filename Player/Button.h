/**
* @file Button.h
* @brief 自定义按钮控件
* @authors yandaoyang
* @date 2021/01/01
* @note
*/

#pragma once
#include <QPushButton>

class BubbleTip;
class Button : public QPushButton
{
    Q_OBJECT

public:
    explicit Button(QWidget* parent = Q_NULLPTR);
    ~Button();

public:
    void setToolTip(const QString&);
    void setTipText(const QString&);
    QString toolTip() const;

protected:
    bool eventFilter(QObject*, QEvent*) Q_DECL_OVERRIDE;

private:
    BubbleTip* _tip{ nullptr };
};

