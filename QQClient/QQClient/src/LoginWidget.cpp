#include<QGridLayout>
#include<QLayout>
#include<QMessageBox>
#include<QEvent>
#include<QDebug>
#include<QPixmap>
#include<QGridLayout>
#include<QRadioButton>
#include<QCheckBox>
#include<functional>

#include <Windows.h>

#include"MainWidget.h"
#include"LoginWidget.h"
#include "RegisterDialog.h"
#include "NetService.h"
LoginWidget::LoginWidget(QWidget *parent) :
    QWidget(parent)
{
    initView();
    connect(_loginButton,&QPushButton::clicked,std::bind(&LoginWidget::onClickLoginCallback,this));

	NetService::getInstance()->registerOnRecvLoginResultCallback([&](int res,const QString& name) {
		
		_isAreadySendLoginMsg = false;
		if (res == 3) {
			QMessageBox::information(this, "server err", "服务器在线人数达到上限，请稍后重试..");
			return;
		}
		if (res == -1) {
			QMessageBox::information(this, "server err", "服务器出错");
			exit(-1);
		}
		if (res == 1) {
			QMessageBox::information(this, "not found", "用户名不存在，请先注册");
			return;
		}
		if (res == 2) {
			QMessageBox::information(this, "not found", "您输入的密码有误，请重新输入");
			return;
		}


		//登陆成功
		_mainWidget->setWindowTitle("QQ:"+QString::number(_qq)+"  name:"+name);
		_mainWidget->_qq = _qq;
		_mainWidget->_qqName = name;
	
		_mainWidget->show();
	
		this->close();
		this->deleteLater();
	});
}

bool LoginWidget::eventFilter(QObject * obj, QEvent *e)
{
    if(obj->objectName()=="closeButton"){

        QPushButton* button=dynamic_cast<QPushButton*>(obj);
        if(e->type() == QEvent::Type::HoverEnter){
             button->setIcon(*_close_button_hover);
        }else if(e->type() == QEvent::Type::HoverLeave){
             button->setIcon(*_close_button_normal);
        }
    }

    return QWidget::eventFilter(obj,e);
}

void LoginWidget::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
    if(event->y()<=181){
        _isMousrPressInTop=true;
        _prePos=event->globalPos();
    }else{
       _isMousrPressInTop=false;
    }

}

void LoginWidget::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
    if(_isMousrPressInTop){
      QPoint delter=event->globalPos()-_prePos;
       this->setGeometry(this->x()+delter.x(),this->y()+delter.y(),size().width(),size().height());
        _prePos=event->globalPos();
    }
}

void LoginWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);
    _isMousrPressInTop=false;
}


void LoginWidget::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Enter){
        this->onClickLoginCallback();
    }
}

void LoginWidget::initView()
{

    //设置窗口没有边框
    setWindowFlags(Qt::FramelessWindowHint);
    resize(QSize(430,330));

    /*mainLayout*/
    QVBoxLayout* mainLayout=new QVBoxLayout(this);
    mainLayout->setMargin(0);


    _close_button_hover = new QPixmap(":/QQClient/Resources/close_button.png");
    _close_button_normal = new QPixmap(":/QQClient/Resources/close_button_normal.png");

    /* top animation picture*/
    QLabel *topAnimationLabel=new QLabel();
    topAnimationLabel->setPixmap(QPixmap(":/QQClient/Resources/top_animation.png"));
    mainLayout->addWidget(topAnimationLabel);
    mainLayout->setAlignment(Qt::AlignTop);

    /*close button*/
    QPushButton* closeButton=new QPushButton(topAnimationLabel);
    closeButton->setObjectName("closeButton");
    closeButton->setIconSize(_close_button_normal->size());
    closeButton->setIcon(*_close_button_normal);
    closeButton->setGeometry(QRect(401,0,29,29));
    closeButton->installEventFilter(this);
    closeButton->setIconSize(_close_button_normal->size());
    connect(closeButton,&QPushButton::clicked,[&](){ this->close(); });



    /*下面部分*/
    QGridLayout* gridLayout=new QGridLayout();
    mainLayout->addLayout(gridLayout);
    gridLayout->setMargin(0);

    //gridLayout->setColumnStretch(0,2);
    QLabel* headImageLabel=new QLabel();
    QPixmap headImag(":/QQClient/Resources/login_left_button_view.png");

    headImageLabel->setPixmap(headImag);
    headImageLabel->setFixedSize(QSize(134,150));
    gridLayout->addWidget(headImageLabel,0,0,4,1,Qt::AlignRight);


    _lineEditUserQQ=new QLineEdit(0);
    _lineEditUserQQ->setFixedHeight(30);
    _lineEditUserQQ->setFixedWidth(190);
    _lineEditUserQQ->setStyleSheet("font-size:20px");
    gridLayout->addWidget(_lineEditUserQQ,0,1,1,2,Qt::AlignBottom);

    gridLayout->setContentsMargins(0,0,0,0);
    gridLayout->setSpacing(0);
    _lineEditPasswd=new QLineEdit(0);
    _lineEditPasswd->setStyleSheet("font-size:20px");
    _lineEditPasswd->setFixedHeight(30);
    _lineEditPasswd->setFixedWidth(190);
    _lineEditPasswd->setEchoMode(QLineEdit::Password);
    gridLayout->addWidget(_lineEditPasswd,1,1,1,2,Qt::AlignTop);

    QLabel* _registerQQLabel=new QLabel();
	_registerQQLabel->setText(u8"<a href='#'>注册账号</a>");

    _registerQQLabel->setStyleSheet("color:#2685E3");
	connect(_registerQQLabel, &QLabel::linkActivated, std::bind(&LoginWidget::onRegisterLinkClicked, this));
    gridLayout->addWidget(_registerQQLabel,0,3,1,1,Qt::AlignCenter);

    QLabel* _reFindPasswdLabel=new QLabel();
    _reFindPasswdLabel->setText("找回密码");
    _reFindPasswdLabel->setStyleSheet("color:#2685E3");
    gridLayout->addWidget(_reFindPasswdLabel,1,3,1,1,Qt::AlignCenter);
    gridLayout->setColumnStretch(0,134);
    gridLayout->setColumnStretch(1,96);
    gridLayout->setColumnStretch(2,96);
    gridLayout->setColumnStretch(3,105);


    _loginButton = new QPushButton;
    QPixmap loginPixmap(":/QQClient/Resources/login_button.png");
    _loginButton->setIcon(loginPixmap);
    _loginButton->setIconSize(loginPixmap.size());
    _loginButton->setFixedSize(loginPixmap.size());
    gridLayout->addWidget(_loginButton,3,1,1,2,Qt::AlignTop);


    QCheckBox* _rememberPasswdRadioButton=new QCheckBox();
    _rememberPasswdRadioButton->setChecked(true);
    QLabel *      _rememberPasswdLabel =new QLabel(" 记住密码");
    _rememberPasswdLabel->setStyleSheet("color:#656565");
    QHBoxLayout*   rememberPasswdLayout=new QHBoxLayout();
    rememberPasswdLayout->addWidget(_rememberPasswdRadioButton);
    rememberPasswdLayout->addWidget(_rememberPasswdLabel);
    gridLayout->addLayout(rememberPasswdLayout,2,1,1,1,Qt::AlignLeft);


    QCheckBox* _autoLoginRadioButton=new QCheckBox();

    QLabel *      _autoLoginPasswdLabel =new QLabel(" 自动登陆");
    _autoLoginPasswdLabel->setStyleSheet("color:#656565");
    QHBoxLayout*   autoLoginLayout=new QHBoxLayout();
    autoLoginLayout->setMargin(10);
    autoLoginLayout->addWidget(_autoLoginRadioButton);
    autoLoginLayout->addWidget(_autoLoginPasswdLabel);
    gridLayout->addLayout(autoLoginLayout,2,2,1,1,Qt::AlignRight);
    this->setStyleSheet("background-color:#EBF2F9");

}

void LoginWidget::onClickLoginCallback()
{

	if (_isAreadySendLoginMsg) {
		QMessageBox::information(this, tr("错误"), tr("已经成功发送登陆请求了，请稍等片刻"));
		return;
	}

    if (_lineEditUserQQ->text().isEmpty())//如果lineEditUserID控件内容为空，提示用户错误
    {
        QMessageBox::information(this, tr("错误"), tr("qq不能为空"));
        _lineEditUserQQ->setFocus();
        return;
    }

    bool ok;
    _qq = _lineEditUserQQ->text().toInt(&ok);
    if (!ok)//如果lineEditUserID控件内容不是数字，提示用户错误
    {
        QMessageBox::information(this, tr("error"), tr("qq号码必须为数字"));
        _lineEditUserQQ->setFocus();
        return;
    }

    if(_qq < 0)//如果用户输入的userid小于0，大于255，提示错误，程序退出
    {
        QMessageBox::information(this, tr("错误"), tr("无效用户QQ号"));
        _lineEditUserQQ->setFocus();
        return;
    }

    _pwd = _lineEditPasswd->text();

    if(_pwd.isEmpty()){
        QMessageBox::information(this, tr("error"), tr("密码不能为空"));
        _lineEditPasswd->setFocus();
        return;
    }

	

	if (NetService::getInstance()->sendLoginMessage(_qq, _pwd)) {
		_isAreadySendLoginMsg = true;
	}
	else {
		QMessageBox::information(this, tr("error"), tr("发送登陆请求失败"));
		return;
	}

}


void LoginWidget::onRegisterLinkClicked() {
	RegisterDialog regDlg;
	regDlg.exec();
	NetService::getInstance()->registerOnRecvRegisterResCallback(nullptr);
}