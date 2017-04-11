
#include <QApplication>
#include"MainWidget.h"
#include<QDebug>
#include"LoginWidget.h"
#include "NetService.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


	MainWidget* mainWidget = new MainWidget();

	/*main loop*/
	int ret = a.exec();

	/*clear*/
	delete mainWidget;
    return ret;
}
