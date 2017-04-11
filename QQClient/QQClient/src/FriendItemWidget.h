#pragma once

#include <QWidget>
#include "QQUser.h"
#include "common.h"
class FriendItemWidget:public QWidget
{
	Q_OBJECT
public:
	FriendItemWidget(const QQUser& user);
	~FriendItemWidget();

public:
	QQUser _user;
};

