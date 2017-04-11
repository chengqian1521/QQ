#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "socket_common.h"
#include "socket_util.h"
int socket_client_init(void **handle, int contime, int sendtime, int revtime,
		int nConNum)
{
	int ret = 0;
	//判断传入的参数
	if (handle == NULL || contime < 0 || sendtime < 0 || revtime < 0)
	{
		ret = Sck_ErrParam; //赋值预先定义的错误。
		printf(
				"func sckCliet_init() err: %d, check  (handle == NULL ||contime<0 || sendtime<0 || revtime<0)\n",
				ret);
		return ret;
	}
	//定义结构体
    SocketHandle *tmp = (SocketHandle *) malloc(sizeof(SocketHandle));
	if (tmp == NULL)
	{
		ret = Sck_ErrMalloc;
		printf("func sckCliet_init() err: malloc %d\n", ret);
		return ret;
	}

	tmp->contime = contime;
	tmp->sendtime = sendtime;
	tmp->revtime = revtime;
	tmp->arrayNum = nConNum;
	*handle = tmp;
	return ret;
}


int socket_client_connect(void *handle, char *ip, int port, int *connfd)
{

	int ret = 0;
    SocketHandle *tmp = NULL;
	if (handle == NULL || ip == NULL || connfd == NULL || port < 0
			|| port > 65537)
	{
		ret = Sck_ErrParam;
		printf(
				"func sckCliet_getconn() err: %d, check  (handle == NULL || ip==NULL || connfd==NULL || port<0 || port>65537) \n",
				ret);
		return ret;
	}

	//
	int sockfd;
	sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd < 0)
	{
		ret = errno;
		printf("func socket() err:  %d\n", ret);
		return ret;
	}

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = inet_addr(ip);

    tmp = (SocketHandle*) handle;

	/*
	 ret = connect(sockfd, (struct sockaddr*) (&servaddr), sizeof(servaddr));
	 if (ret < 0)
	 {
	 ret = errno;
	 printf("func connect() err:  %d\n", ret);
	 return ret;
	 }
	 */

    ret = connect_timeout(sockfd, (struct sockaddr_in*) (&servaddr),
			(unsigned int) tmp->contime);
	if (ret < 0)
	{
		if (ret == -1 && errno == ETIMEDOUT)
		{
			ret = Sck_ErrTimeOut;
			return ret;
		} else
		{
			printf("func connect_timeout() err:  %d\n", ret);
		}
	}

	*connfd = sockfd;

	return ret;

}


int socket_client_send(void *handle, int connfd, unsigned char *data, int datalen)
{
	int ret = 0;

    SocketHandle *tmp = NULL;
    tmp = (SocketHandle *) handle;
    ret = write_timeout_check(connfd, tmp->sendtime);
	if (ret == 0)
	{
		int writed = 0;
		unsigned char *netdata = (unsigned char *) malloc(datalen + 4);
		if (netdata == NULL)
		{
			ret = Sck_ErrMalloc;
			printf("func sckClient_send() mlloc Err:%d\n ", ret);
			return ret;
		}
		int netlen = htonl(datalen);
		memcpy(netdata, &netlen, 4);
		memcpy(netdata + 4, data, datalen);

        writed = write_fix_n(connfd, netdata, datalen + 4);
		if (writed < (datalen + 4))
		{
			if (netdata != NULL)
			{
				free(netdata);
				netdata = NULL;
			}
			return writed;
		}

	}

	if (ret < 0)
	{
		//失败返回-1，超时返回-1并且errno = ETIMEDOUT
		if (ret == -1 && errno == ETIMEDOUT)
		{
			ret = Sck_ErrTimeOut;
			printf("func sckClient_send() mlloc Err:%d\n ", ret);
			return ret;
		}
		return ret;
	}

	return ret;
}

int socket_client_recv(void *handle, int connfd, unsigned char *out, int *outlen)
{

	int ret = 0;
    SocketHandle *tmpHandle = (SocketHandle *) handle;

	if (handle == NULL || out == NULL)
	{
		ret = Sck_ErrParam;
		printf("func sckClient_rev() timeout , err:%d \n", Sck_ErrTimeOut);
		return ret;
	}

    ret = read_timeout_check(connfd, tmpHandle->revtime); //bugs modify bombing
	if (ret != 0)
	{
		if (ret == -1 || errno == ETIMEDOUT)
		{
			ret = Sck_ErrTimeOut;
			printf("func sckClient_rev() timeout , err:%d \n", Sck_ErrTimeOut);
			return ret;
		} else
		{
			printf("func sckClient_rev() timeout , err:%d \n", Sck_ErrTimeOut);
			return ret;
		}
	}

	int netdatalen = 0;
    ret = read_fix_n(connfd, &netdatalen, 4); //读包头 4个字节
	if (ret == -1)
	{
		printf("func readn() err:%d \n", ret);
		return ret;
	} else if (ret < 4)
	{
		ret = Sck_ErrPeerClosed;
		printf("func readn() err peer closed:%d \n", ret);
		return ret;
	}

	int n;
	n = ntohl(netdatalen);
    ret = read_fix_n(connfd, out, n); //根据长度读数据
	if (ret == -1)
	{
		printf("func readn() err:%d \n", ret);
		return ret;
	} else if (ret < n)
	{
		ret = Sck_ErrPeerClosed;
		printf("func readn() err peer closed:%d \n", ret);
		return ret;
	}

	*outlen = n;

	return 0;
}

int socket_client_destory(void *handle)
{
    if (!handle)
	{
        return Sck_ErrParam;
	}
    free(handle);
	return 0;
}

int socket_client_close_connection(int connfd)
{
    if (connfd < 0)
	{
        return Sck_ErrParam;
    }

    int ret=0;
    ret=close(connfd);
    if(ret==-1){
        perror("close err");
        return Sck_ErrPeerClosed;
    }
    return 0;
}


int socket_server_init(int port, int *listenfd)
{
	int ret = 0;
	int mylistenfd;
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //返回一个新的socket描述符
	mylistenfd = socket(PF_INET, SOCK_STREAM, 0);
	if (mylistenfd < 0)
	{
        perror("func socket() err:");
		return ret;
	}
	int on = 1;
	ret = setsockopt(mylistenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if (ret < 0)
	{
        perror("func setsockopt() err:");
		return ret;
	}
	ret = bind(mylistenfd, (struct sockaddr*) &servaddr, sizeof(servaddr));
	if (ret < 0)
	{
        perror("func bind() err:");
		return ret;
	}
	ret = listen(mylistenfd, SOMAXCONN);
	if (ret < 0)
	{
        perror("func listen() err:");
		return ret;
	}
	*listenfd = mylistenfd;
	return 0;
}


int socket_server_accept(int listenfd, int *connfd/*out*/, int timeout)
{
	int ret = 0;

    ret = accept_timeout(listenfd, NULL, (unsigned int) timeout);
	if (ret < 0)
	{
		if (ret == -1 && errno == ETIMEDOUT)
		{
			ret = Sck_ErrTimeOut;
            perror("func accept_timeout() timeout err:");
			return ret;
		} else
		{
            perror("func accept_timeout() err:");
			return ret;
		}
	}

	*connfd = ret;
	return 0;
}

int socket_server_send(int connfd, unsigned char *data, int datalen, int timeout)
{
	int ret = 0;
    //写时超时检测
    ret = write_timeout_check(connfd, timeout);
	if (ret == 0)
	{
		int writed = 0;
		//分配内存空间
		unsigned char *netdata = (unsigned char *) malloc(datalen + 4);
		if (netdata == NULL)
		{
			ret = Sck_ErrMalloc;
			printf("func sckServer_send() mlloc Err:%d\n ", ret);
			return ret;
		}
		//将本地数据转换为网络数据  ;小端===》大端
		int netlen = htonl(datalen);
		//将数据的长度加到数据包的头4字节处
		memcpy(netdata, &netlen, 4);
		//将数据打包到新的数据包中。
		memcpy(netdata + 4, data, datalen);
        //发送数据
		//writed为成功发送的数据的字节长度。
        writed = write_fix_n(connfd, netdata, datalen + 4);
		//直到数据分包 封装 发送完成之后，返回
		if (writed < (datalen + 4))
		{
			//释放内存
			if (netdata != NULL)
			{
				free(netdata);
				netdata = NULL;
			}
			return writed;
		}

	}
    //检测超时
	if (ret < 0)
	{
		//失败返回-1，超时返回-1并且errno = ETIMEDOUT
		//链接超时
		if (ret == -1 && errno == ETIMEDOUT)
		{
			ret = Sck_ErrTimeOut;
			printf("func sckServer_send() mlloc Err:%d\n ", ret);
			return ret;
		}
		return ret;
	}

	return ret;
}

int socket_server_recv(int connfd, unsigned char *out, int *outlen, int timeout)
{

	int ret = 0;
    //检测传入的参数是否是有效的参数。
	if (out == NULL || outlen == NULL)
	{
		ret = Sck_ErrParam;
		printf("func sckClient_rev() timeout , err:%d \n", Sck_ErrTimeOut);
		return ret;
	}
    //检测是否可读，防止阻塞假死，一个链接的等待时间是1.5倍的RTT 一个RTT 75秒
    ret = read_timeout_check(connfd, timeout); //bugs modify bombing
	if (ret != 0)
	{
		if (ret == -1 || errno == ETIMEDOUT)
		{
			ret = Sck_ErrTimeOut;
			printf("func sckClient_rev() timeout , err:%d \n", Sck_ErrTimeOut);
			return ret;
		} else
		{
			printf("func sckClient_rev() timeout , err:%d \n", Sck_ErrTimeOut);
			return ret;
		}
	}
	/*
	 * 防止粘包
	 * */

    //定义收取的数据的长度，以用来获取收取数据的长度，初始化为0；
	//通过调用readn返回数据的长度
	int netdatalen = 0;
    ret = read_fix_n(connfd, &netdatalen, 4); //读包头 4个字节
	if (ret == -1)
	{
		printf("func readn() err:%d \n", ret);
		return ret;
	} else if (ret < 4)
	{
		ret = Sck_ErrPeerClosed;
		printf("func readn() err peer closed:%d \n", ret);
		return ret;
	}
	int n;
	//将网络数据转换为本地数据，大端===>小端
	n = ntohl(netdatalen);
    ret = read_fix_n(connfd, out, n); //根据长度读数据
	if (ret == -1)
	{
		printf("func readn() err:%d \n", ret);
		return ret;
	} else if (ret < n)
	{
		ret = Sck_ErrPeerClosed;
		printf("func readn() err peer closed:%d \n", ret);
		return ret;
	}
    //抛出需要读取的字节长度。
	*outlen = n;
	return 0;
}

//服务器端环境释放 
int socket_server_destory(int listen_fd)
{
    if(listen_fd<0){
        return Sck_ErrParam;
    }
    int ret=0;
    ret=close(listen_fd);
    if(ret==-1){
        perror("close err");
        return -1;
    }
    return ret;
}
