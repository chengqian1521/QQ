#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/epoll.h>
#include <arpa/inet.h>
#include <iostream>
#include <assert.h>


#include "QQServer.h"
#include "util.h"
#include "socket_common.h"
#include "socket_util.h"
#include "common.h"
#include "QQUserDao.h"

#include "boost/algorithm/string.hpp"

QQServer::QQServer(int port):_qqUserDao(new QQUserDao())
{

    int ret=0;
    ret=socket_server_init(port,&listen_st);//创建server端socket
    if(ret==-1){
        perror("socket_server_init err");
        exit(-1);
    }

}

QQServer::~QQServer()
{
    int ret=socket_server_destory(listen_st);
    if(ret==-1){
        perror("socket_server_destory err");
        exit(-1);
    }
    delete _qqUserDao;
}





void QQServer::broadcastUserState() //向socket_client[]数组中所有client广播用户状态消息
{

    int msg_len = MESSAGE_MAIN_LEN+64*_fd_to_qq.size();
    msg_t *res_msg = (msg_t*)malloc(msg_len);
    memset(res_msg,0,msg_len);

    res_msg->size = msg_len;
    res_msg->type = UPDATE_STUSTAS;
    *(int*)res_msg->head = _fd_to_qq.size();

    char *start = res_msg->body;
    for(auto &pair : _qq_to_qqUser){
        QQUser& u = pair.second;

        *(int*)start = u._qq;

        start += sizeof(int);

        strcpy(start,u._name.data());

        start+=60;

    }
    for(auto &pair: _fd_to_qq){
        if(sendCompletMessage(pair.first,res_msg)==-1){
            ErrPrint("%s",strerror(errno));
            break;
        }
    }
    free(res_msg);
}



void QQServer::handleLoginMessage(int src_st, const msg_t *msg)
{


    int msg_len=MESSAGE_MAIN_LEN+64;
    msg_t * res_msg = (msg_t *)malloc(msg_len);
    memset(res_msg,0,msg_len);
    res_msg->type = LOGIN_RES;
    res_msg->size = msg_len;

    int ret;
    QQUser user;
    if(_fd_to_qq.size()>=MaxOnlineCount){

        res_msg->head[0] = 3;
        if(sendCompletMessage(src_st,res_msg)==-1){
            ErrPrint("%s",strerror(errno));
        }
        goto END;
    }


	

    ret=_qqUserDao->findQQUserByQQ(msg->src,&user);


    if(ret == -1){
        //出错
        res_msg->head[0] = -1;
        if(sendCompletMessage(src_st,res_msg)==-1){
            ErrPrint("%s",strerror(errno));
        }
        goto END;
    }


    if(ret == 1){
        //该用户没找到
        res_msg->head[0] = 1;
        if(sendCompletMessage(src_st,res_msg)==-1){
            ErrPrint("%s",strerror(errno));
        }
        goto END;
    }
    if(user._pwd!=msg->body){
        //密码错误
        res_msg->head[0] = 2;
        if(sendCompletMessage(src_st,res_msg)==-1){
            ErrPrint("%s",strerror(errno));
        }
        goto END;
    }

    //登陆成功
    res_msg->head[0] = 0;
    strcpy((char*)res_msg->body,user._name.data());
    if(sendCompletMessage(src_st,res_msg)==-1){
        ErrPrint("%s",strerror(errno));
    }

    user._socket_fd=src_st;
    addOnlineUser(src_st, user); //将登录密码验证通过的客户端client安装到socket_client[]的数组中
    broadcastUserState(); //向socket_client数组中所有socket广播用户状态消息


    goto END;
END:
    free(res_msg);

}

void QQServer::handleSendMessage(int st, const msg_t *msg)
{
    int dest = msg->dest;
    auto it = _qq_to_qqUser.find(dest);
    if(it == _qq_to_qqUser.end()){
        //对方不在线。。回一个错误包

        return;
    }
    QQUser &u = it->second;
    if(sendCompletMessage(u._socket_fd,msg)==-1){
        ErrPrintErrStr;
    }

}




void QQServer::user_logout(int fd) //client socket连接断开
{
   int qq=_fd_to_qq[fd];

   std::cout<<"qq用户:"<<qq<<" 下线"<<std::endl;

   _qq_to_qqUser.erase(qq);
   _fd_to_qq.erase(fd);
   broadcastUserState();
}


void QQServer::addOnlineUser(int socket_fd,  QQUser &qqUser)
{
    auto it=_qq_to_qqUser.find(qqUser._qq);


    if(it!=_qq_to_qqUser.end()){
        //该用户已经在其他地方登陆

        std::cout<<"qq用户:"<<qqUser._qq<<" 顶号"<<std::endl;

        //让原来的客户端断开连接
        if(::close(it->second._socket_fd)==-1){
            ErrPrint("%s",strerror(errno));
        }

        qqUser._socket_fd = socket_fd;

        _fd_to_qq.erase(it->second._socket_fd);
        _qq_to_qqUser.erase(it);
    }

    std::cout<<"qq用户:"<<qqUser._qq<<" 上线"<<std::endl;

    _qq_to_qqUser[qqUser._qq]=qqUser;
    _fd_to_qq[socket_fd] = qqUser._qq;

}
int QQServer::recvCompletMessage(int st)
{

    int msg_len = -1;

    ssize_t rc=read_fix_n(st,&msg_len,4);
    if(rc<4){
        return -1;
    }

    struct msg_t *msg = (msg_t *)new char[msg_len];

    rc = read_fix_n(st, (unsigned char *)msg+4, msg_len - 4);//接收来自client socket发送来的消息

    if (rc < msg_len - 4)//接收失败
	{
        return -1;
    }

    msg->size = msg_len;

    std::cout<<"recv a new message:type="<<msg->type<<" size="<<msg->size<<"  src="<<msg->src<<"   dest="<<msg->dest
            <<std::endl;

    switch (msg->type)
    {
    case SEND: //send消息
        handleSendMessage(st,msg);
        break;
    case LOGIN: //login消息
        handleLoginMessage(st,msg);
        break;
    case REGISTER: //注册消息
        handleRegsterMessage(st,msg);
        break;
    default: //无法识别的消息
        ErrPrint("recv a undefine message\n");
    }
    delete msg;
    return rc;
}

int QQServer::sendCompletMessage(int target_fd, const msg_t *msg)
{
    int ret=write_fix_n(target_fd,(void*)msg,msg->size);

#ifdef QQDEBUG
    std::cout<<"send a  message:size="<<msg->size<<"  type="<<msg->type<<"  src="<<msg->src<<"   dest="<<msg->dest
            <<"  "<<std::endl;
#endif
    if(ret == -1)
        return -1;

    return 0;
}

int QQServer::handleRegsterMessage(int sender_fd,const msg_t *msg)
{

     /* 从msg.body中挖出pwd和name */
    std::string body(msg->body);

    int idx=body.find(';');

    std::string pwd(body.begin(),body.begin()+idx);
    std::string name(body.begin()+idx+1,body.end());


    QQUser user(msg->src,pwd,name);

    int ret=0;
    ret=_qqUserDao->addQQUser(&user);

    msg_t *ok_msg = (msg_t *)new char[MESSAGE_MAIN_LEN];
    ok_msg->size = MESSAGE_MAIN_LEN;
    ok_msg->type =MessageType::REGISTER_RES;
    ok_msg->head[0] = (char)ret;
    ret = sendCompletMessage(sender_fd,ok_msg);
    delete ok_msg;
    return ret;
}



int QQServer::run(){

    struct epoll_event ev;
    epoll_event* events=new epoll_event[MaxConnectCount+1]; //声明epoll_event结构体的变量,ev用于注册事件,数组用于回传要处理的事件
    if(set_io_to_nonblock(listen_st)==-1){
        //把socket设置为非阻塞方式
         ErrPrintErrStr;
    }

    int epfd = epoll_create(MaxConnectCount+1); //生成用于处理accept的epoll专用的文件描述符
    if(epfd==-1){
        ErrPrintErrStr;
        exit(-1);
    }


    ev.data.fd = listen_st; //设置与要处理的事件相关的文件描述符
	ev.events = EPOLLIN | EPOLLERR | EPOLLHUP; //设置要处理的事件类型	

    //注册epoll事件
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, listen_st, &ev)==-1){
        ErrPrintErrStr;
        exit(-1);
    }

     int st = 0;
    while (true)
	{
        int nfds = epoll_wait(epfd, events, MaxConnectCount+1, -1); //等待epoll事件的发生
		if (nfds == -1)
		{
            ErrPrintErrStr;
			break;
		}

        for (int i = 0; i < nfds; ++i)
		{

			if (events[i].data.fd == listen_st) //监测到一个SOCKET用户连接到了绑定的SOCKET端口，建立新的连接。
			{
                int ret=0;

                ret=socket_server_accept(listen_st,&st,0);//将来自client端的socket描述符设置为非阻塞
                if (ret < 0)
				{
                    ErrPrintErrStr;
                    exit(-1);
				}
                std::cout<<"recv a new connect fd="<<st<<std::endl;

                set_io_to_nonblock(st);

                ev.data.fd = st;
                ev.events = EPOLLIN | EPOLLERR | EPOLLHUP; //设置要处理的事件类型

                //将来自client端的socket描述符加入epoll
                if(epoll_ctl(epfd, EPOLL_CTL_ADD, st, &ev))
                {
                    ErrPrintErrStr;
                    exit(-1);
                }
                continue;

			}
			if (events[i].events & EPOLLIN) //socket收到数据
			{

				st = events[i].data.fd;
                int ret=recvCompletMessage(st);
                if(ret<=0){
                    socket_fd_close_before(st);
                    if(close(st) == -1){
                        ErrPrintErrStr;
                    }
                }
			}
            if (events[i].events & EPOLLERR)   //socket错误。
			{
                std::cout<<"EPOLLERR"<<std::endl;
			}

            if (events[i].events & EPOLLHUP)   //socket被挂断。
			{
                std::cout<<"EPOLLHUP"<<std::endl;
			}
		}
	}

    delete []events;
    close(epfd);
    return 0;
}

void QQServer::socket_fd_close_before(int fd)
{
    auto it = _fd_to_qq.find(fd);
    if(it == _fd_to_qq.end())
        return;
     user_logout(fd);
}

