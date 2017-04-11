
#include <QMessageBox>
#include <QHostAddress>
#include "MainWidget.h"
#include "LoginWidget.h"
#include "ChatWidget.h"
#include "LoginWidget.h"
#include "NetService.h"
#include "common.h"
#include "utils.h"
#include <QHash>
#include <QListWidgetItem>

#include "FriendItemWidget.h"

//主窗口的信号过滤函数，watched代表是哪个控件触发了信号，event代表触发了具体什么信号
bool MainWidget::eventFilter(QObject *watched, QEvent *event)
{
	if (watched->objectName() == "closeButton") {
		QPushButton* button = dynamic_cast<QPushButton*>(watched);
		if (event->type() == QEvent::Type::HoverEnter) {
			button->setIcon(QPixmap(":/QQClient/Resources/main_close_hover.png"));
		}
		else if (event->type() == QEvent::Type::HoverLeave) {
			button->setIcon(QPixmap(":/QQClient/Resources/main_close_normal.png"));
		}
	}
	else if (watched->objectName() == "friendItem") {
		FriendItemWidget* w = dynamic_cast<FriendItemWidget*>(watched);
		//qDebug() << event->type();
		if (event->type() == QEvent::Type::Enter) {
			QPalette palette;			
			palette.setBrush(QPalette::Background, QBrush(QColor(0xA1C4CF)));
			w->setPalette(palette);
		}
		else if (event->type() == QEvent::Type::Leave) {
			QPalette palette;
			palette.setBrush(QPalette::Background, QBrush(QColor(0xF0F0F0)));
			w->setPalette(palette);
		}
		else if (event->type() == QEvent::Type::MouseButtonDblClick) {
			int qq=w->_user._qq;
			auto it=_qqToCharWidgetMap.find(qq);
			ChatWidget* chatWidget = nullptr;
			if (it == _qqToCharWidgetMap.end()) {
				_qqToCharWidgetMap.insert(std::make_pair(qq,chatWidget=new ChatWidget(_qq,qq,w->_user._name)));
			}
			else {
				chatWidget = it->second;
			}

			chatWidget->show();

		}


	}
    return QWidget::eventFilter(watched, event);//其他信号交给父类进行默认处理
}

void MainWidget::initView()//初始化所有的toolBtn
{

	resize(290, 600);//设置主窗口大小为宽300，高600
	setWindowIcon(QPixmap(":/QQClient/Resources/3.png"));
	this->setWindowFlags(Qt::FramelessWindowHint);
	
	QVBoxLayout* mainVBoxLayout = new QVBoxLayout(this);
	mainVBoxLayout->setAlignment(Qt::AlignTop);
	mainVBoxLayout->setMargin(0);
	mainVBoxLayout->setContentsMargins(0, 0, 0, 0);
	mainVBoxLayout->setSpacing(0);

	/*init title*/
	initTitleView(mainVBoxLayout);

	/*init top Panel*/
	initTopPanel(mainVBoxLayout);
	

	/*init navigation_bar*/
	QLabel *navigationBar = new QLabel();
	navigationBar->setPixmap(QString(":/QQClient/Resources/navigation_bar.png"));
	mainVBoxLayout->addWidget(navigationBar);

	initFriendPanel(mainVBoxLayout);
	
	mainVBoxLayout->addStretch(1);

	QLabel* bottomLabel = new QLabel();
	bottomLabel->setPixmap(QPixmap(":/QQClient/Resources/main_bottum.png"));
	mainVBoxLayout->addWidget(bottomLabel);
	
}

void MainWidget::initTitleView(QVBoxLayout* mainVBoxLayout) {
	QLabel* topTitle = new QLabel();
	topTitle->setPixmap(QPixmap(":/QQClient/Resources/main_title.png"));
	topTitle->setMargin(0);

	QPushButton* closeButton = new QPushButton(topTitle);
	QPixmap closeNormalPixmap(":/QQClient/Resources/main_close_normal.png");
	closeButton->setContentsMargins(0, 0, 0, 0);
	closeButton->setIcon(closeNormalPixmap);
	closeButton->setIconSize(closeNormalPixmap.size());
	closeButton->setFixedSize(closeNormalPixmap.size());
	closeButton->installEventFilter(this);
	closeButton->connect(closeButton, &QPushButton::clicked, std::bind(&QPushButton::close, this));
	closeButton->move(290 - 28, 0);
	closeButton->setObjectName("closeButton");
	mainVBoxLayout->addWidget(topTitle);
	
}

void MainWidget::initTopPanel(QVBoxLayout* mainVBoxLayout) {
	QLabel* topWidget = new QLabel();

	//设置背景颜色
	topWidget->setAutoFillBackground(true);
	QPalette palette;
	palette.setColor(QPalette::Background, QColor(0x288ADD));
	topWidget->setPalette(palette);
	topWidget->setFixedSize(290, 95);

	_headLabel = new QLabel();
		
	QHBoxLayout* hBoxLayout = new QHBoxLayout(topWidget);
	hBoxLayout->addWidget(_headLabel);

	  _qqNameLabel = new QLabel("");
	hBoxLayout->addWidget(_qqNameLabel);

	QLabel*  qqStatus = new QLabel("<span style='color:green;font-size:18px'>[在线]</span>");
	hBoxLayout->addWidget(qqStatus);

	mainVBoxLayout->addWidget(topWidget);

}
MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent)
{
    
	LoginWidget *loginWidget = new LoginWidget();
	loginWidget->show();
	NetService::getInstance()->setParent(this);
	loginWidget->_mainWidget = this;
    initView();

	netService->registerOnRecvUpdateUsersStateCallback([&](std::vector<QQUser>& onlineUsers) {	
		for (auto& u : onlineUsers) {
			if(u._qq == _qq)
				continue;
			_qqToFriendInfoMap.insert(std::make_pair(u._qq,u));
		}
		
		
		const QList<QObject*> objs = _friendList->children();

		for (auto obj : objs) {
			QWidget* w = dynamic_cast<QWidget*>(obj);
			_friendListLayout->removeWidget(w);
			w->deleteLater();
		}
		for (auto &u : onlineUsers) {
			if (u._qq == _qq)
				continue;
			FriendItemWidget* w = new FriendItemWidget({ u._qq,u._name });
			w->installEventFilter(this);
			_friendListLayout->addWidget(w);
		}
		
	});

	netService->registerOnRecvNewMessageCallback([&](int src,const QString& content) {
		
		QQUser* u = findFriendInfoByQQ(src);
		if (!u) {
			//用户不在线
			return;
		}
		ChatWidget* chatWidget = findChatWidgetByQQ(src);
		if (!chatWidget) {
			//窗口还没打开
			addChatWidgetToMap(src, chatWidget = new ChatWidget(_qq, src, u->_name));		
		}
		chatWidget->addMessageToBrowser(u->_name, content);
		chatWidget->show();
	});

}

MainWidget::~MainWidget()
{

}

void MainWidget::mousePressEvent(QMouseEvent *event)
{
	QWidget::mousePressEvent(event);
	if (event->y() <= 124) {
		_isMousrPressInTop = true;
		_prePos = event->globalPos();
	}
	else {
		_isMousrPressInTop = false;
	}

}

void MainWidget::mouseMoveEvent(QMouseEvent *event)
{
	QWidget::mouseMoveEvent(event);
	if (_isMousrPressInTop) {
		QPoint delter = event->globalPos() - _prePos;
		this->setGeometry(this->x() + delter.x(), this->y() + delter.y(), size().width(), size().height());
		_prePos = event->globalPos();
	}
}

void MainWidget::mouseReleaseEvent(QMouseEvent *event)
{
	QWidget::mouseReleaseEvent(event);
	_isMousrPressInTop = false;
}

void MainWidget::showEvent(QShowEvent* ev) {
	QWidget::showEvent(ev);
	QPixmap headImage(":/QQClient/Resources/" + QString::number(utils::getImageIndexByQString(_qqName)) + ".png");

	_headLabel->setPixmap(headImage);
	_qqNameLabel->setText(_qqName+"("+QString::number(_qq)+")");	
}
void MainWidget::initFriendPanel(QVBoxLayout* mainVBoxLayout) {


	QWidget* friendPanel = new QWidget();
	mainVBoxLayout->addWidget(friendPanel);

	QHBoxLayout* friendPanelLayout = new QHBoxLayout(friendPanel);
	friendPanel->setContentsMargins(0, 0, 0, 0);
	friendPanelLayout->setMargin(0);
	friendPanelLayout->setSpacing(0);
	initFriendList(friendPanelLayout);
	

	/*init slider*/
	QSlider* slider = new QSlider();
	slider->setValue(100);
	slider->setFixedSize(QSize(12,420));
	slider->setRange(0,100);
	connect(slider, &QSlider::valueChanged, [&](int value) {

		//计算出多出来的的像素
		int height = _friendList->size().height();
		int all = _friendList->children().size() * FriendItemFixedHeight;

		int beyond = all - height;
		if (beyond <= 0)
			return;
		
		_friendList->move(_friendList->pos().x(),- beyond*float(100-value) / 100);
	});

	friendPanelLayout->addWidget(slider,12);
}
void MainWidget::initFriendList(QHBoxLayout* friendPanelLayout) {
	
	_friendList = new QWidget();
	_friendList->setFixedHeight(420);
	_friendListLayout = new QVBoxLayout(_friendList);
	_friendListLayout->setAlignment(Qt::AlignTop);
	_friendListLayout->setMargin(0);
	_friendListLayout->setSpacing(0);
	friendPanelLayout->addWidget(_friendList, 278);
}




void MainWidget::closeEvent(QCloseEvent *event)//窗口在close之前会自动调用closeEvent函数
{
#if 0
    QMessageBox::StandardButton button = QMessageBox::question(this, tr("退出程序"),
                                                               QString(tr("是否退出?")),
                                                               QMessageBox::Yes | QMessageBox::No);

    if (button == QMessageBox::Yes)
    {
        event->accept();  //接受退出信号，程序退出
    }
    else
    {
        event->ignore();   //忽略退出信号，程序继续运行
    }
#endif
}



