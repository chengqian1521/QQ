#pragma once
#include <QString>
class QQUser
{
public:
	QQUser(int qq,const QString& name);
	QQUser() = default;
	~QQUser();

	int _qq;
	QString _name;


};

