#pragma once
#include <QDialog>
#include "ui_LoginDialog.h"

class LoginDialog : public QDialog
{
   Q_OBJECT

public:
   static LoginDialog& Get() {
      static LoginDialog dlg;
      return dlg;
   }
   explicit LoginDialog(QWidget* parent = Q_NULLPTR);
   ~LoginDialog();

public:
   static QString UserName() { return username; }

private slots:
   void loginReq();
   void loginRes(QByteArray);
   void toRegister();

protected:
   void initWidgets();
   void connectSlots();
   bool showEmptyLab();
   void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
   Ui::LoginDialog ui;
   QPushButton* _btnEye { nullptr };
   QLabel* _labEmptyUser{ nullptr };
   QLabel* _labEmptyPass{ nullptr };
   static inline QString username{ QString() };
};

