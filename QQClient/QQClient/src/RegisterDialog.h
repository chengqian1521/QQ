#pragma once
#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
#pragma execution_character_set("utf-8")
class RegisterDialog:public QDialog
{
	Q_OBJECT
public:
	RegisterDialog();
	~RegisterDialog();

	void onClickedRegisterCallback();

	QLineEdit* _qqLineEdit;
	QLineEdit* _qqNameLineEdit;
	QLineEdit* _qqPwdLineEdit;
	QLineEdit* _qqConfirmPwdLineEdit;

	bool _isAlreadySendRegisterMessage = false;
};

