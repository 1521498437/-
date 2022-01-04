#pragma once
#include <QDialog>
#include "ui_RegistDialog.h"

class RegistDialog : public QDialog
{
   Q_OBJECT

public:
static RegistDialog& Get() {
   static RegistDialog dlg;
   return dlg;
}
   explicit RegistDialog(QWidget* parent = Q_NULLPTR);
   ~RegistDialog();

private slots:
   void registReq();
   void registRes(QByteArray);
   void toLogin();

protected:
   void initWidgets();
   void connectSlots();
   bool showWrongLab();
   void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
   Ui::RegistDialog ui;
   bool sameAcc = true;
   QPushButton* _btnEye1{ nullptr };
   QPushButton* _btnEye2{ nullptr };
   QLabel* _labWrongUser{ nullptr };
   QLabel* _labWrongPass1{ nullptr };
   QLabel* _labWrongPass2{ nullptr };
};

