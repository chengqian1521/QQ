#include "FriendItemWidget.h"
#include "QHBoxLayout"
#include "utils.h"
#include <QLabel>
FriendItemWidget::FriendItemWidget(const QQUser& user):_user(user)
{
	setFixedSize(278, FriendItemFixedHeight);
	this->setObjectName("friendItem");
	setAutoFillBackground(true);
	QHBoxLayout* main = new QHBoxLayout(this);
	main->setMargin(0);
	main->setSpacing(0);
	main->setAlignment(Qt::AlignLeft);

	QLabel* headImageLabel = new QLabel();
	headImageLabel->setPixmap(QPixmap(":/QQClient/Resources/"+QString::number(utils::getImageIndexByQString(user._name))+".png"));
	main->addWidget(headImageLabel);

	main->addWidget(new QLabel(user._name+"("+QString::number(_user._qq)+")"));
	
}


FriendItemWidget::~FriendItemWidget()
{ 
}
