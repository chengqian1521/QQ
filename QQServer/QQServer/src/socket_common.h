#ifndef _SCK_CLINT_H_
#define _SCK_CLINT_H_
#ifdef __cplusplus
extern "C"
{
#endif
//错误码定义  
#define Sck_Ok             	0
#define Sck_BaseErr   		3000
#define Sck_ErrParam                	(Sck_BaseErr+1)
#define Sck_ErrTimeOut                	(Sck_BaseErr+2)
#define Sck_ErrPeerClosed               (Sck_BaseErr+3)
#define Sck_ErrMalloc			   		(Sck_BaseErr+4)
typedef struct _SocketHandle
{
	int sockArray[100]; //定义socket池数组
    int arrayNum;       //数组大小
    int sockfd;         //socket句柄
    int contime;        //链接超时时间
    int sendtime;       //发送超时时间
    int revtime;        //接受超时时间
}SocketHandle;



/**
 *@brief 客户端环境初始化
 *@param  handle  在函数内部分配内存，socket结构体
 *@param  contime    链接超时时间
 *@param  sendtime  发送超时时间
 *@param  revtime   接受超时时间
 *@param  nConNum   链接池的数目
 *@return 成功返回Sck_Ok（0）  出错返回自定义错误
 * */
int socket_client_init(void **handle,  int contime, int sendtime, int revtime, int nConNum);

/**
 * @brief  连接服务器
 * @param handle 句柄
 * @param ip     服务器ip
 * @param port   服务器端口
 * @param connfd 连接成功后返回的客户端socket描述符
 * @return 成功返回0 出错返回自定义错误
 */
int socket_client_connect(void *handle, char *ip, int port, int *connfd);

/**
 * @brief 关闭客户端socket fd
 * @param connfd
 * @return 成功返回0 出错返回自定义错误
 */
int socket_client_close_connection(int connfd);



/**
 * @brief socket_client_send
 * @param handle
 * @param connfd
 * @param data
 * @param datalen
 * @return 成功返回0 出错返回自定义错误
 */
int socket_client_send(void *handle, int  connfd,  unsigned char *data, int datalen);

/**
 * @brief socket_client_recv
 * @param handle
 * @param connfd
 * @param out
 * @param outlen
 * @return 成功返回0 出错返回自定义错误
 */
int socket_client_recv(void *handle, int  connfd, unsigned char *out, int *outlen); //1

/**
 * @brief socket_client_destory
 * @param handle
 * @return
 */
int socket_client_destory(void *handle);



/////////////////////////////////////////////////////////////////////////////////////
//服务器端初始化
/**
 * @brief 服务器端的socket初始化
 * @param port 绑定的端口
 * @param listenfd 监听的socket fd，传出参数
 * @return 如果成功返回0 ，失败返回<0,并设置errno
 * */
int socket_server_init(int port, int *listenfd/*out*/);

/**
 * @brief 从三次握手的队列中取出一个连接，若队列为空，则阻塞等待timeout时间
 * @param： listenfd 监听的sock fd
 * @param:	timeout  定义的超时时间
 * @return：如果成功返回0 ，失败返回<0,
 * */
int socket_server_accept(int listenfd, int *connfd,  int timeout);



/**
 * @brief 服务器端发送报文,发送报文，并进行了粘包处理，超时处理。
 * @param connfd connfd 链接的socket描述符
 * @param data          发送的数据  ，传入数据，在内部重新打包封装。
 * @param datalen       要发送的数据的长度
 * @param timeout       定义的超时时间
 * @return 如果成功返回0 ，失败返回<0 或者 成功发送的数据的字节大小。
 */
int socket_server_send(int connfd,  unsigned char *data, int datalen, int timeout);




/**
 * @brief //服务器端端接受报文,接受报文，并进行了粘包处理。
 * @param connfd 链接的socket描述符
 * @param out    读取的内容，在外部分配内存
 * @param outlen 读取到内容的长度。
 * @param timeout定义的超时时间
 * @return 如果成功返回0 ，失败返回<0 或者错误码。
 */
int socket_server_recv(int  connfd, unsigned char *out, int *outlen,  int timeout); //1

/**
 * @brief 服务器端环境释放
 * @param handle
 * @return
 */
int socket_server_destory(int listen_fd);



#ifdef __cplusplus
}
#endif
#endif
