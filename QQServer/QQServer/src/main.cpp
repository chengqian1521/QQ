#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "util.h"
#include "QQServer.h"
#include "QQUserDao.h"
#include "QQUser.h"
#include <iostream>


int main(int arg, char *args[])
{
#if 0


#endif
	if (arg < 2)//如果没有参数，main函数返回
	{
		printf("usage:qqserverd port\n");
		return -1;
	}

	int iport = atoi(args[1]);
	if (iport == 0)
	{
		printf("port %d is invalid\n", iport);
		return -1;
	}
#ifndef QQDebug
    util::setDaemon();//进入daemon状态
    util::signal1(SIGINT, util::catch_Signal);
    util::signal1(SIGPIPE, util::catch_Signal);
#endif

    QQServer qqServer(iport);
    return qqServer.run();
}

