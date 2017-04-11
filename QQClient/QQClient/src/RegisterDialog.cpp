#include "RegisterDialog.h"
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <functional>
#include <QPushButton>
#include <QMessageBox>
#include <QRegExp>

#include "NetService.h"

RegisterDialog::RegisterDialog()
{
	QGridLayout* layout = new QGridLayout(this);

	QLabel *qqLabel = new QLabel("qq����");
	_qqLineEdit = new QLineEdit();
	_qqLineEdit->setPlaceholderText("�������Զ����qq����");

	layout->addWidget(qqLabel, 0, 0);
	layout->addWidget(_qqLineEdit, 0, 1);

	QLabel *qqName = new QLabel("qq�ǳ�");
	 _qqNameLineEdit = new QLineEdit();
	layout->addWidget(qqName, 1, 0);
	layout->addWidget(_qqNameLineEdit, 1, 1);


	QLabel *qqPwdLabel = new QLabel("����");
	 _qqPwdLineEdit = new QLineEdit();
	_qqPwdLineEdit->setEchoMode(QLineEdit::Password);
	layout->addWidget(qqPwdLabel, 2, 0);
	layout->addWidget(_qqPwdLineEdit, 2, 1);

	QLabel *qqConfirmPwdLabel = new QLabel("ȷ������");
	_qqConfirmPwdLineEdit = new QLineEdit();
	_qqConfirmPwdLineEdit->setEchoMode(QLineEdit::Password);
	layout->addWidget(qqConfirmPwdLabel, 3, 0);
	layout->addWidget(_qqConfirmPwdLineEdit, 3, 1);

	QPushButton *registerButton = new QPushButton("ע��");
	QPushButton *cancleButton = new QPushButton("ȡ��");

	connect(registerButton, &QPushButton::clicked, std::bind(&RegisterDialog::onClickedRegisterCallback,this));
	connect(cancleButton, &QPushButton::clicked,
			std::bind(&QDialog::close,this));

	layout->addWidget(registerButton, 4, 0);
	layout->addWidget(cancleButton, 4, 1);
	

	//���öԻ��򱳾���ɫ��ͼƬ
	setAutoFillBackground(true);
	QPalette palette;
	//palette.setColor(QPalette::Background, QColor(Qt::blue));
	palette.setBrush(QPalette::Background, QBrush(QPixmap(":/QQClient/Resources/1.jpg")));
	setPalette(palette);

	NetService::getInstance()->registerOnRecvRegisterResCallback([&](int res) {
		_isAlreadySendRegisterMessage = false;
		if (res == 0) {
			QMessageBox::information(this, "ok", "��ϲ��ע��ɹ�");
			return;
		}
		if (res == 1) {
			QMessageBox::information(this, "err", "��QQ�˻��Ѿ���ʹ�ã���ע��������");
			return;
		}
		if (res == -1) {
			QMessageBox::information(this, "err", "����������");
			return;
		}
	});

}


RegisterDialog::~RegisterDialog()
{
}
void RegisterDialog::onClickedRegisterCallback(){

	

	bool ok;
	int qq=_qqLineEdit->text().toInt(&ok);
	if (!ok||qq<1) {
		QMessageBox::information(this,"input error","qq���������1~"+ QString::number(INT_MAX) +"������");
		return;
	}

	QString name = _qqNameLineEdit->text();
	if (name.size() < 1 || name.size() > 10) {
		QMessageBox::information(this, "input error", "qq�ǳƳ��ȱ�����1~18");
		return;
	}
	if (name.contains(';')) {
		QMessageBox::information(this, "input error", "qq�ǳƲ��ܺ��зֺ�");
		return;
	}

	/* ������ʽ ^[a-zA-Z]\w{5,17}$
	ƥ�� ����ĸ��ͷ��������6~18֮�䣬ֻ�ܰ����ַ������ֺ��»���
                                                                     */

	QString pwd = _qqPwdLineEdit->text();
	QRegExp regExp("^\\w{4,18}$");
	if (!regExp.exactMatch(pwd)) {
		QMessageBox::information(this, "input error", "QQ���볤����4~18֮�䣬ֻ�ܰ����ַ������ֺ��»���");
		return;
	}

	QString confirmPwd = _qqConfirmPwdLineEdit->text();
	if (pwd != confirmPwd) {
		QMessageBox::information(this, "input error", "���ε��������벻һ��");
		return;
	}

	if (!NetService::getInstance()->isConnectedOk()){
		if (NetService::getInstance()->getTcpState() == QAbstractSocket::UnconnectedState)
		{
			NetService::getInstance()->connectToServer();
		}
	}

	if (_isAlreadySendRegisterMessage) {
		QMessageBox::information(this, "net err", "���ڴ���ղŵķ����������Ե�");
		return;
	}

	
	int ret=NetService::getInstance()->sendRegisterMessage(qq, pwd, name);
	if (ret == -1) {
		//дʧ��
		QMessageBox::information(this, "net err", "д����ʧ��");
		return;
	}

	_isAlreadySendRegisterMessage = true;

}