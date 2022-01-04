#pragma once
#include <QTextEdit>

class TextEdit : public QTextEdit
{
	Q_OBJECT

public:
	explicit TextEdit(QWidget* parent = Q_NULLPTR);
	~TextEdit();

protected:
	void paintEvent(QPaintEvent*) Q_DECL_OVERRIDE;
};

