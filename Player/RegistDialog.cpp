#include "RegistDialog.h"
#include <QAction>
#include <QTimer>
#include <QPainter>
#include <QRegExp>
#include <QRegExpValidator>
#include "LiveSocket.h"
#include "Service.h"
#pragma execution_character_set("utf-8")

RegistDialog::RegistDialog(QWidget *parent) :
    QDialog(parent)
{
    ui.setupUi(this);
    initWidgets();
    connectSlots();
}

RegistDialog::~RegistDialog()
{
}

void RegistDialog::initWidgets()
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);//窗口去掉标题栏
    setAttribute(Qt::WA_TranslucentBackground, true);//窗口设置透明
    //添加编辑框图标
    QAction* nameAction = new QAction(ui.edtName);
    nameAction->setIcon(QIcon(":/App/Res/icon/user.png"));
    ui.edtName->addAction(nameAction,QLineEdit::LeadingPosition);////
    QAction* pwdAction = new QAction(ui.edtPwd);
    pwdAction->setIcon(QIcon(":/App/Res/icon/password.png"));
    ui.edtPwd->addAction(pwdAction,QLineEdit::LeadingPosition);//////
    QAction* re_pwdAction = new QAction(ui.edtRePwd);
    re_pwdAction->setIcon(QIcon(":/App/Res/icon/re_password.png"));
    ui.edtRePwd->addAction(re_pwdAction,QLineEdit::LeadingPosition);
    //添加小眼睛按钮
    _btnEye1 = new QPushButton(ui.edtPwd);
    //去掉小眼睛的tab焦点
    _btnEye1->setFocusPolicy(Qt::NoFocus);
    _btnEye1->setIcon(QIcon(":/App/Res/icon/eye_no.png"));
    _btnEye1->setGeometry(ui.edtPwd->width() - ui.edtPwd->height(), y(), ui.edtPwd->height(), ui.edtPwd->height());
    _btnEye1->setFlat(true);
    _btnEye1->setStyleSheet("border-radius:10px;background-color:rgba(255,255,255,0)");
    _btnEye2 = new QPushButton(ui.edtRePwd);
    //去掉小眼睛的tab焦点
    _btnEye2->setFocusPolicy(Qt::NoFocus);
    _btnEye2->setIcon(QIcon(":/App/Res/icon/eye_no.png"));
    _btnEye2->setGeometry(ui.edtRePwd->width() - ui.edtRePwd->height(), y(), ui.edtRePwd->height(), ui.edtRePwd->height());
    _btnEye2->setFlat(true);
    _btnEye2->setStyleSheet("border-radius:10px;background-color:rgba(255,255,255,0)");
    //屏蔽输入法(不输入中文)
    ui.edtPwd->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui.edtRePwd->setAttribute(Qt::WA_InputMethodEnabled, false);
    //屏蔽输入空格
    auto validator = new QRegExpValidator(QRegExp("^[\\S]{0,}$"), this);
    ui.edtPwd->setValidator(validator);
    ui.edtRePwd->setValidator(validator);
    //只能输入中文和英文
    auto validator2 = new QRegExpValidator(QRegExp("^[a-zA-Z\u4e00-\u9fa5]{0,}$"), this);
    ui.edtName->setValidator(validator2);
    //初始化用于提示错误信息的标题
    _labWrongUser = new QLabel("昵称不能为空!",ui.widgetEdtName);
    _labWrongUser->setGeometry(90,0,150,13);
    _labWrongUser->setStyleSheet("color:rgb(255,0,0);");
    _labWrongUser->hide();
    _labWrongPass1 = new QLabel("密码不能为空!",ui.widgetEdtPwd);
    _labWrongPass1->setGeometry(90,0,150,13);
    _labWrongPass1->setStyleSheet("color:rgb(255,0,0);");
    _labWrongPass1->hide();
    _labWrongPass2 = new QLabel("确认密码不能为空!",ui.widgetEdtRePwd);
    _labWrongPass2->setGeometry(90,0,150,13);
    _labWrongPass2->setStyleSheet("color:rgb(255,0,0);");
    _labWrongPass2->hide();
}

void RegistDialog::connectSlots()
{
    //小眼睛功能处理
   connect(_btnEye1, &QPushButton::pressed, [this] {
      ui.edtPwd->setEchoMode(QLineEdit::Normal);
      _btnEye1->setIcon(QIcon(":/App/Res/icon/eye.png"));
   });
   connect(_btnEye1, &QPushButton::released, [this] {
      ui.edtPwd->setEchoMode(QLineEdit::Password);
      _btnEye1->setIcon(QIcon(":/App/Res/icon/eye_no.png"));
   });
   connect(_btnEye2, &QPushButton::pressed, [this] {
      ui.edtRePwd->setEchoMode(QLineEdit::Normal);
      _btnEye2->setIcon(QIcon(":/App/Res/icon/eye.png"));
   });
   connect(_btnEye2, &QPushButton::released, [this] {
      ui.edtRePwd->setEchoMode(QLineEdit::Password);
      _btnEye2->setIcon(QIcon(":/App/Res/icon/eye_no.png"));
   });

   //对话框跳转处理
   connect(ui.btnCancle, &QPushButton::clicked, this, &RegistDialog::toLogin);

   //注册业务处理
   connect(ui.btnReg, &QPushButton::clicked, this, &RegistDialog::registReq);
   connect(&LiveSocket::Get(), &LiveSocket::registRes, this, &RegistDialog::registRes);

   //错误信息处理
   connect(ui.edtName, &QLineEdit::textEdited, _labWrongUser, &QLabel::hide);
   connect(ui.edtPwd, &QLineEdit::textEdited, _labWrongPass1, &QLabel::hide);
   connect(ui.edtRePwd, &QLineEdit::textEdited, _labWrongPass2, &QLabel::hide);
}

void RegistDialog::registReq()
{
   if (!showWrongLab())
   {
      ui.btnReg->setText("请稍后...");
      LiveSocket::Get().registReq(ui.edtName->text(), ui.edtPwd->text());
   }
}

void RegistDialog::registRes(QByteArray arr)
{
   auto res = (srv::RegistRes*)arr.data();
   if (res->success)
   {
      ui.btnReg->setText("注册成功!");
      QTimer::singleShot(600, this, &RegistDialog::toLogin);
   }
   else
   {
      ui.btnReg->setText("注册失败!此用户名已被注册");
      QTimer::singleShot(1300, [this] {
         ui.btnReg->setText("注册");
      });
   }
}

void RegistDialog::toLogin()
{
   ui.edtName->clear();
   ui.edtPwd->clear();
   ui.edtRePwd->clear();
   _labWrongUser->hide();
   _labWrongPass1->hide();
   _labWrongPass2->hide();
   ui.btnReg->setText("注册");
   close();
}

bool RegistDialog::showWrongLab()
{
   bool ret = false;

   if (ui.edtName->text().isEmpty()) {
      _labWrongUser->show();
      ret = true;
   } else {
      _labWrongUser->hide();
   }

   if (ui.edtPwd->text().isEmpty()) {
      _labWrongPass1->setText("密码不能为空!");
      _labWrongPass1->show();
      ret = true;
   } else {
      if (ui.edtPwd->text().length() < 8) {
         _labWrongPass1->setText("密码长度8~16位!");
         _labWrongPass1->show();
         ret = true;
      } else {
         _labWrongPass1->hide();
      }
   }

   if (ui.edtRePwd->text().isEmpty()) {
      _labWrongPass2->setText("确认密码不能为空!");
      _labWrongPass2->show();
      ret = true;
   } else {
      if (ui.edtPwd->text() != ui.edtRePwd->text()) {
         _labWrongPass2->setText("两次密码不一致!");
         _labWrongPass2->show();
         ret = true;
      } else {
         _labWrongPass2->hide();
      }
   }

   return ret;
}

void RegistDialog::paintEvent(QPaintEvent *event)
{
   QWidget::paintEvent(event);
   QPainter painter(this);
   painter.setRenderHint(QPainter::Antialiasing, true);
   painter.setBrush(QBrush(QColor(255, 255, 255)));
   painter.setPen(Qt::transparent);
   QRect rect = this->rect();
   rect.setWidth(rect.width() - 1);
   rect.setHeight(rect.height() - 1);
   painter.drawRoundedRect(rect, 15, 15);
}
