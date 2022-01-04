#include "BaseWindow.h"
#include "TitleBar.h"
#include "CommonWindow.h"
#include <windows.h>
#include <windowsx.h>
#include <QMouseEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QPainterPath>
#include <QPainter>
#include <QtMath>
#include <QDebug>

struct BaseWindowPrivate
{
    //�����϶�
    bool   mDrag{ false };
    QPoint mDragPos;

    //�������
    QRect  mLocation;

    //�߿���Ӱ
    int    mShadow;

    //��������صı���
    bool   mZoom{ false };
    QRect  mZoomLocation;
    QPoint mZoomPos;
};

BaseWindow::BaseWindow(QWidget* content)
	: QWidget(nullptr), p(new BaseWindowPrivate) //�����踸����
{
	ui.setupUi(this);
   ui.gridLayout->getContentsMargins(&p->mShadow, nullptr, nullptr, nullptr);
   ui.gridLayout->addWidget(content, 2, 1, 1, 1);
   setWindowFlags(windowFlags() | Qt::FramelessWindowHint /*| Qt::WindowStaysOnTopHint*/);
   //ȥ������������Ԥ��ͼ�Ҽ��˵���Ϊ�˽������쳣���⣬���ڿ����������������
   setWindowFlags(windowFlags() & ~Qt::WindowSystemMenuHint);
   //���ñ���ɫΪ͸���������Ӱ�а����⡣
   setAttribute(Qt::WA_TranslucentBackground, true);
   p->mLocation = geometry();

   //ui.labLeft->installEventFilter(this);
   //ui.labTop->installEventFilter(this);
   ui.labRight->installEventFilter(this);
   ui.labBottom->installEventFilter(this);
   //ui.labLT->installEventFilter(this);
   //ui.labRT->installEventFilter(this);
   ui.labRB->installEventFilter(this);
   //ui.labLB->installEventFilter(this);

   //ui.labLeft->setCursor(Qt::SizeHorCursor);
   //ui.labTop->setCursor(Qt::SizeVerCursor);
   ui.labRight->setCursor(Qt::SizeHorCursor);
   ui.labBottom->setCursor(Qt::SizeVerCursor);
   //ui.labLT->setCursor(Qt::SizeFDiagCursor);
   //ui.labRT->setCursor(Qt::SizeBDiagCursor);
   ui.labRB->setCursor(Qt::SizeFDiagCursor);
   //ui.labLB->setCursor(Qt::SizeBDiagCursor);

   connect(ui.titleBar, &TitleBar::sigShowMin, this, &QWidget::showMinimized);
   connect(ui.titleBar, &TitleBar::sigShowMax, this, &BaseWindow::onShowMax);///////////
   connect(ui.titleBar, &TitleBar::sigRestore, this, &BaseWindow::onShowNormal);
   connect(ui.titleBar, &TitleBar::sigCloseWin, this, &BaseWindow::sigClose);
   connect(ui.titleBar, &TitleBar::sigCloseWin, this, &QWidget::close);

   ui.titleBar->init(); //!!!
}

BaseWindow::~BaseWindow()
{
    delete p;
}

void BaseWindow::setResizable(bool flag)
{
    ui.labBottom->setEnabled(flag);
    ui.labLB->setEnabled(flag);
    ui.labLeft->setEnabled(flag);
    ui.labLT->setEnabled(flag);
    ui.labRB->setEnabled(flag);
    ui.labRight->setEnabled(flag);
    ui.labRT->setEnabled(flag);
    ui.labTop->setEnabled(flag);
}

int BaseWindow::shadowWidth() const
{
    return p->mShadow;
}

TitleBar* BaseWindow::titleBar() const
{
    return ui.titleBar;
}

bool BaseWindow::eventFilter(QObject* obj, QEvent* e)
{
    if (obj == ui.labBottom || obj == ui.labLB || obj == ui.labLeft || obj == ui.labLT
        || obj == ui.labRB || obj == ui.labRight || obj == ui.labRT || obj == ui.labTop)
    {
        //ʵ���϶��ҡ��¡����½����Ŵ���
        //��������ϵ����Ѷȣ�Ŀǰδʵ��
        if (e->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent* event = (QMouseEvent*)e;
            if (event->buttons() & Qt::LeftButton)
            {
                if (ui.titleBar->isMax())
                {
                    //����󻯣��Ͳ������϶�
                    return true;
                }
                p->mZoom = true;
                p->mZoomLocation = geometry();
                p->mZoomPos = event->globalPos();
                return true;
            }
        }
        else if (e->type() == QEvent::MouseMove)
        {
            QMouseEvent* event = (QMouseEvent*)e;
            if (p->mZoom && (event->buttons() & Qt::LeftButton))
            {
                int dx = event->globalPos().x() - p->mZoomPos.x();
                int dy = event->globalPos().y() - p->mZoomPos.y();
                QRect rc = p->mZoomLocation;
                /*if (obj == ui.labLeft) ////�����ϡ����¡����ϡ������������˸����Ϊ��resize��move
                {
                    //if (dx >= 0) return true;
                    rc.setLeft(rc.left() + dx);
                }
                else if (obj == ui.labTop) ////
                {
                    if (dy >= 0) return true;
                    rc.setTop(rc.top() + dy);
                }
                else*/
                if (obj == ui.labRight)
                {
                    rc.setRight(rc.right() + dx);
                }
                else if (obj == ui.labBottom)
                {
                    rc.setBottom(rc.bottom() + dy);
                }
                /*else if (obj == ui.labLT) ////
                {
                    if (dx >= 0 || dy >= 0) return true;
                    rc.setLeft(rc.left() + dx);
                    rc.setTop(rc.top() + dy);
                }
                else if (obj == ui.labRT) ////
                {
                    if (dx >= 0 || dy >= 0) return true;
                    rc.setRight(rc.right() + dx);
                    rc.setTop(rc.top() + dy);
                }*/
                else if (obj == ui.labRB)
                {
                    rc.setRight(rc.right() + dx);
                    rc.setBottom(rc.bottom() + dy);
                }
                /*else if (obj == ui.labLB) ////
                {
                    if (dx >= 0 || dy >= 0) return true;
                    rc.setLeft(rc.left() + dx);
                    rc.setBottom(rc.bottom() + dy);
                }*/
                setGeometry(rc);
                //update();
                return true;
            }
        }
        else if (e->type() == QEvent::MouseButtonRelease)
        {
            p->mZoom = false;
            return true;
        }
    }
    return QObject::eventFilter(obj, e);
}

void BaseWindow::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton && 
        ui.titleBar->geometry().contains(e->pos()))
    {
        p->mDrag = true;
        p->mDragPos = e->globalPos() - pos();
        e->accept();
    }
}

void BaseWindow::mouseMoveEvent(QMouseEvent* e)
{
    if (p->mDrag && (e->buttons() && Qt::LeftButton))
    {
        if (ui.titleBar->isMax())
        {
            //��¼��갴��ʱ����ڽ���λ�õı���
            QPoint point(QCursor::pos());
            double rateX = point.x() / (double)(width() - ui.titleBar->buttonsWidth());
            double rateY = point.y() / (double)height();
            //�ָ������ǰ��λ�úͳߴ�
            ui.titleBar->onBtnMaxClicked();
            //����λ�õ�ƥ�������ϼ�¼�ı���
            setGeometry(point.x() - (width() - ui.titleBar->buttonsWidth()) * rateX,
                point.y() - height() * rateY, width(), height());
            p->mDragPos = e->globalPos() - pos();
            return; //���п���
        }
        move(e->globalPos() - p->mDragPos);
        e->accept();
    }
}

void BaseWindow::mouseReleaseEvent(QMouseEvent* event)
{
    p->mDrag = false;
}

void BaseWindow::onShowMax()
{
    p->mLocation = geometry();
    ////ע�����µ���仰���ò�͸��(����false)���ñ�����ȫ��
    //setAttribute(Qt::WA_TranslucentBackground, false);
    ui.labBottom->hide();
    ui.labLB->hide();
    ui.labLeft->hide();
    ui.labLT->hide();
    ui.labRB->hide();
    ui.labRight->hide();
    ui.labRT->hide();
    ui.labTop->hide();
    setGeometry(qApp->desktop()->availableGeometry());
    emit this->sigShowMax();
}

void BaseWindow::onShowNormal()
{
    //setAttribute(Qt::WA_TranslucentBackground, true);
    ui.labBottom->show();
    ui.labLB->show();
    ui.labLeft->show();
    ui.labLT->show();
    ui.labRB->show();
    ui.labRight->show();
    ui.labRT->show();
    ui.labTop->show();
    setGeometry(p->mLocation);
    emit this->sigRestore();
}

//qApp->exit()

/*bool BaseWindow::nativeEvent(const QByteArray& eventType, void* message, long* result)
{ 
    //���촰��ûЧ��
    int m_nBorder = 5;
    Q_UNUSED(eventType)
        MSG* param = static_cast<MSG*>(message);

    switch (param->message)
    {
    case WM_NCHITTEST:
    {
        int nX = GET_X_LPARAM(param->lParam) - this->geometry().x();
        int nY = GET_Y_LPARAM(param->lParam) - this->geometry().y();

        //if mouse in the child Qwidget of mainfrme, we should not handle it
        if (childAt(nX, nY) != NULL)
            return QWidget::nativeEvent(eventType, message, result);

        * result = HTCAPTION;

        // if mouse in the border of mainframe,we zoom mainframe
        if ((nX > 0) && (nX < m_nBorder))
            *result = HTLEFT;

        if ((nX > this->width() - m_nBorder) && (nX < this->width()))
            *result = HTRIGHT;

        if ((nY > 0) && (nY < m_nBorder))
            *result = HTTOP;

        if ((nY > this->height() - m_nBorder) && (nY < this->height()))
            *result = HTBOTTOM;

        if ((nX > 0) && (nX < m_nBorder) && (nY > 0)
            && (nY < m_nBorder))
            *result = HTTOPLEFT;

        if ((nX > this->width() - m_nBorder) && (nX < this->width())
            && (nY > 0) && (nY < m_nBorder))
            *result = HTTOPRIGHT;

        if ((nX > 0) && (nX < m_nBorder)
            && (nY > this->height() - m_nBorder) && (nY < this->height()))
            *result = HTBOTTOMLEFT;

        if ((nX > this->width() - m_nBorder) && (nX < this->width())
            && (nY > this->height() - m_nBorder) && (nY < this->height()))
            *result = HTBOTTOMRIGHT;

        if (*result == HTCAPTION)
        {
            return false;
        }
        return true;
    }
    }
    return QWidget::nativeEvent(eventType, message, result);
}*/

void BaseWindow::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    if (!ui.titleBar->isMax())
    {
        painter.fillRect(QRect(
            p->mShadow, p->mShadow,
            width() - 2 * p->mShadow,
            height() - 2 * p->mShadow),
            QBrush(Qt::black) //��App����ɫΪ��ɫ ��߲���Բ�ǣ�����������
        );
        QColor color(0, 0, 0);
        //int rgb[] = { 180, 192, 204, 216, 228, 242 };
        for (int i = 0; i < p->mShadow; i++)
        {
            //color.setRgb(rgb[i], rgb[i], rgb[i]);
            //color.setAlpha(45 - qSqrt(i) * 17);
            //��Ƶ��ʾ����������Ӱ��ǳ
            color.setAlpha(120 - qSqrt(i) * 48); 
            painter.setPen(color);
            //Բ����Ӱ�߿�
            painter.drawRoundedRect(
                p->mShadow - i, p->mShadow - i,
                width() - (p->mShadow - i) * 2,
                height() - (p->mShadow - i) * 2,
                2, 2
            );
        }
    }
    else
    {   //��App����ɫΪ��ɫ
        painter.fillRect(QRect(0, 0, width(), height()), QBrush(Qt::black));
    }
}
