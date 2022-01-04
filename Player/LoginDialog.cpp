#include "LoginDialog.h"
#include <QAction>
#include <QPainter>
#include <QTimer>
#include <QRegExp>
#include <QRegExpValidator>
#include "LiveSocket.h"
#include "Service.h"
#include "RegistDialog.h"
#pragma execution_character_set("utf-8")
//#include "dialog_tip.h"
//#include "dialog_register.h"

LoginDialog::LoginDialog(QWidget* parent) :
    QDialog(parent)//, _regDlg(new RegistDialog(this))
{
    ui.setupUi(this);
    initWidgets();
    connectSlots();
}

LoginDialog::~LoginDialog()
{
}

void LoginDialog::initWidgets()
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);//窗口去掉标题栏
    setAttribute(Qt::WA_TranslucentBackground, true);//窗口设置透明
    //添加编辑框图标
    QAction* userAction = new QAction(ui.edtAcc);
    userAction->setIcon(QIcon(":/App/Res/icon/user.png"));
    ui.edtAcc->addAction(userAction,QLineEdit::LeadingPosition);
    QAction* pwdAction = new QAction(ui.edtPwd);
    pwdAction->setIcon(QIcon(":/App/Res/icon/password.png"));
    ui.edtPwd->addAction(pwdAction,QLineEdit::LeadingPosition);
    //添加小眼睛按钮
    _btnEye = new QPushButton(ui.edtPwd);
    //去掉小眼睛的tab焦点
    _btnEye->setFocusPolicy(Qt::NoFocus);
    _btnEye->setIcon(QIcon(":/App/Res/icon/eye_no.png"));
    _btnEye->setGeometry(ui.edtPwd->width() - ui.edtPwd->height(), y(), ui.edtPwd->height(), ui.edtPwd->height());
    _btnEye->setFlat(true);
    _btnEye->setStyleSheet("border-radius:10px;background-color:rgba(255,255,255,0)");
    //屏蔽输入法(不输入中文)
    ui.edtAcc->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui.edtPwd->setAttribute(Qt::WA_InputMethodEnabled, false);
    //屏蔽输入空格
    auto validator = new QRegExpValidator(QRegExp("^[\\S]{0,}$"), this);
    ui.edtAcc->setValidator(validator);
    ui.edtPwd->setValidator(validator);
    //初始化用于提示错误信息的标题
    _labEmptyUser = new QLabel("账号不能为空!", ui.widgetEdtAcc);
    _labEmptyUser->setGeometry(90,0,150,13);
    _labEmptyUser->setStyleSheet("color:rgb(255,0,0);");
    _labEmptyUser->hide();
    _labEmptyPass = new QLabel("密码不能为空!", ui.widgetEdtPwd);
    _labEmptyPass->setGeometry(90,0,150,13);
    _labEmptyPass->setStyleSheet("color:rgb(255,0,0);");
    _labEmptyPass->hide();
}

void LoginDialog::connectSlots()
{
    //小眼睛功能处理
    connect(_btnEye, &QPushButton::pressed, [this](){
       ui.edtPwd->setEchoMode(QLineEdit::Normal);
       _btnEye->setIcon(QIcon(":/App/Res/icon/eye.png"));
    });
    connect(_btnEye, &QPushButton::released, [this](){
       ui.edtPwd->setEchoMode(QLineEdit::Password);
       _btnEye->setIcon(QIcon(":/App/Res/icon/eye_no.png"));
    });

    //对话框跳转处理
    connect(ui.btnToReg, &QPushButton::clicked, this, &LoginDialog::toRegister);
    connect(ui.btnCancle, &QPushButton::clicked, this, &LoginDialog::close);

    //登录业务处理
    connect(ui.btnLogin, &QPushButton::clicked, this, &LoginDialog::loginReq);
    connect(&LiveSocket::Get(), &LiveSocket::loginRes, this, &LoginDialog::loginRes);

    //错误信息处理
    //connect(ui.edtAcc, &QLineEdit::editingFinished, this, &LoginDialog::showNotNullLab_1);
    //connect(ui.edtPwd, &QLineEdit::editingFinished, this, &LoginDialog::showNotNullLab_2);
    connect(ui.edtAcc, &QLineEdit::textEdited, _labEmptyUser, &QLabel::hide);
    connect(ui.edtPwd, &QLineEdit::textEdited, _labEmptyPass, &QLabel::hide);
}

void LoginDialog::loginReq()
{
   if (!showEmptyLab())
   {
      ui.btnLogin->setText("请稍后...");
      LiveSocket::Get().loginReq(ui.edtAcc->text(), ui.edtPwd->text());
   }
}

void LoginDialog::loginRes(QByteArray arr)
{
   auto res = (srv::LoginRes*)arr.data();
   if(res->success)
   {
      LoginDialog::username = ui.edtAcc->text();
      ui.btnLogin->setText(QString("登录成功!欢迎：%1").arg(LoginDialog::username));
      QTimer::singleShot(1300, this, &LoginDialog::close);
   }
   else
   {
      ui.btnLogin->setText("登录失败！账号或密码错误");
      QTimer::singleShot(1300, [this] {
         ui.btnLogin->setText("登录");
      });
   }
}

void LoginDialog::toRegister()
{
    ui.edtAcc->clear();
    ui.edtPwd->clear();
    _labEmptyUser->hide();
    _labEmptyPass->hide();
    ui.btnLogin->setText("登录");
    RegistDialog::Get().exec();
}

bool LoginDialog::showEmptyLab()
{
   bool ret = false;
   if (ui.edtAcc->text().isEmpty()) {
      _labEmptyUser->show();
      ret = true;
   } else {
      _labEmptyUser->hide();
   }
   if (ui.edtPwd->text().isEmpty()) {
      _labEmptyPass->show();
      ret = true;
   } else {
      _labEmptyPass->hide();
   }
   return ret;
}

void LoginDialog::paintEvent(QPaintEvent *event)
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

