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

	QLabel *qqLabel = new QLabel("qq号码");
	_qqLineEdit = new QLineEdit();
	_qqLineEdit->setPlaceholderText("请输入自定义的qq号码");

	layout->addWidget(qqLabel, 0, 0);
	layout->addWidget(_qqLineEdit, 0, 1);

	QLabel *qqName = new QLabel("qq昵称");
	 _qqNameLineEdit = new QLineEdit();
	layout->addWidget(qqName, 1, 0);
	layout->addWidget(_qqNameLineEdit, 1, 1);


	QLabel *qqPwdLabel = new QLabel("密码");
	 _qqPwdLineEdit = new QLineEdit();
	_qqPwdLineEdit->setEchoMode(QLineEdit::Password);
	layout->addWidget(qqPwdLabel, 2, 0);
	layout->addWidget(_qqPwdLineEdit, 2, 1);

	QLabel *qqConfirmPwdLabel = new QLabel("确认密码");
	_qqConfirmPwdLineEdit = new QLineEdit();
	_qqConfirmPwdLineEdit->setEchoMode(QLineEdit::Password);
	layout->addWidget(qqConfirmPwdLabel, 3, 0);
	layout->addWidget(_qqConfirmPwdLineEdit, 3, 1);

	QPushButton *registerButton = new QPushButton("注册");
	QPushButton *cancleButton = new QPushButton("取消");

	connect(registerButton, &QPushButton::clicked, std::bind(&RegisterDialog::onClickedRegisterCallback,this));
	connect(cancleButton, &QPushButton::clicked,
			std::bind(&QDialog::close,this));

	layout->addWidget(registerButton, 4, 0);
	layout->addWidget(cancleButton, 4, 1);
	

	//设置对话框背景颜色或图片
	setAutoFillBackground(true);
	QPalette palette;
	//palette.setColor(QPalette::Background, QColor(Qt::blue));
	palette.setBrush(QPalette::Background, QBrush(QPixmap(":/QQClient/Resources/1.jpg")));
	setPalette(palette);

	NetService::getInstance()->registerOnRecvRegisterResCallback([&](int res) {
		_isAlreadySendRegisterMessage = false;
		if (res == 0) {
			QMessageBox::information(this, "ok", "恭喜您注册成功");
			return;
		}
		if (res == 1) {
			QMessageBox::information(this, "err", "该QQ账户已经被使用，请注册其他的");
			return;
		}
		if (res == -1) {
			QMessageBox::information(this, "err", "服务器出错");
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
		QMessageBox::information(this,"input error","qq号码必须是1~"+ QString::number(INT_MAX) +"的数字");
		return;
	}

	QString name = _qqNameLineEdit->text();
	if (name.size() < 1 || name.size() > 10) {
		QMessageBox::information(this, "input error", "qq昵称长度必须是1~18");
		return;
	}
	if (name.contains(';')) {
		QMessageBox::information(this, "input error", "qq昵称不能含有分号");
		return;
	}

	/* 正则表达式 ^[a-zA-Z]\w{5,17}$
	匹配 以字母开头，长度在6~18之间，只能包含字符、数字和下划线
                                                                     */

	QString pwd = _qqPwdLineEdit->text();
	QRegExp regExp("^\\w{4,18}$");
	if (!regExp.exactMatch(pwd)) {
		QMessageBox::information(this, "input error", "QQ密码长度在4~18之间，只能包含字符、数字和下划线");
		return;
	}

	QString confirmPwd = _qqConfirmPwdLineEdit->text();
	if (pwd != confirmPwd) {
		QMessageBox::information(this, "input error", "两次的密码输入不一致");
		return;
	}

	if (!NetService::getInstance()->isConnectedOk()){
		if (NetService::getInstance()->getTcpState() == QAbstractSocket::UnconnectedState)
		{
			NetService::getInstance()->connectToServer();
		}
	}

	if (_isAlreadySendRegisterMessage) {
		QMessageBox::information(this, "net err", "正在处理刚才的发送请求，请稍等");
		return;
	}

	
	int ret=NetService::getInstance()->sendRegisterMessage(qq, pwd, name);
	if (ret == -1) {
		//写失败
		QMessageBox::information(this, "net err", "写数据失败");
		return;
	}

	_isAlreadySendRegisterMessage = true;

}