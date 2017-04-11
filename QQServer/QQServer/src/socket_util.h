#ifndef _SCK_UTIL_H_
#define _SCK_UTIL_H_



#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>


#define ERR_EXIT(m) \
  do \
  {  \
    perror(m); \
	exit(EXIT_FAILURE); \
  } \
  while (0)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief        读取固定的n的字节才返回，防止粘包问题和睡眠信号中断。
 * @param fd:    读取的的文件描述符
 * @param buf:   接收缓冲区
 * @param count: 要读取的字节数
 * @return :     成功返回读取到的字节数,读到EOF返回字节小于count，失败返回-1,并且会设置errno
 **/
ssize_t read_fix_n(int fd, void *buf, size_t count);


/**
 * @brief        写入固定的n的字节才返回，防止粘包问题和睡眠信号中断。
 * @param fd:    要写入的文件描述符
 * @param buf:   写入的数据的缓冲区
 * @param count: 要写入数据的长度
 * @return :     成功返回写入的字节数，失败返回-1,并且会设置errno
 **/
ssize_t write_fix_n(int fd, const void *buf, size_t count);


/**
 * @brief          读一行才返回，防止粘包问题和睡眠信号中断。
 * @param fd:      要读的文件描述符
 * @param buf:     接收数据的缓冲区
 * @param maxline: 缓冲区的最大长度
 * @return :       成功失败返回读入的字节数,返回0表示读到EOF,返回-2表示该行超出缓冲区的长度
 *   返回-1表示其他错误，并且会设置errno，意外错误会导致程序退出。
 **/
ssize_t read_line(int sockfd, char *buf, size_t maxline);

/**
 * @brief - 仅仅查看套接字缓冲区数据，但不移除数据
 * @param sockfd: 套接字
 * @param buf:    接收缓冲区
 * @param len:    长度
 * @return 成功返回>0，对方关闭返回0，失败返回-1，并会设置errno
 */
ssize_t recv_peek(int sockfd, char* buf, size_t len);

/**
 * @brief - 读超时检测函数，不含读操作
 * @param fd: 文件描述符
 * @param wait_seconds: 等待超时秒数，如果为0表示不检测超时
 * @return 成功(未超时）返回0，超时返回-1并且errno = ETIMEDOUT,失败返回-1并会设置errno
 */
int read_timeout_check(int fd, unsigned int wait_seconds);

/**
 * @brief write_timeout - 写超时检测函数，不含写操作
 * @param fd: 文件描述符
 * @param wait_seconds: 等待超时秒数，如果为0表示不检测超时
 * @return 成功（未超时）返回0，超时返回-1并且errno = ETIMEDOUT，失败返回-1, 并会设置errno
 */
int write_timeout_check(int fd, unsigned int wait_seconds);


/**
 * @brief - 带超时的accept
 * @param fd: 监听socket套接字
 * @param addr: 输出参数，返回对方地址
 * @param wait_seconds: 等待超时秒数，如果为0表示正常模式
 * @return 成功（未超时）返回已连接套接字，超时返回-1并且errno = ETIMEDOUT，其他错误返回-1并设置errno
 */
int accept_timeout(int fd, struct sockaddr_in *addr/*out*/, unsigned int wait_seconds);


/**
 * @brief  - 带超时的connect
 * @param fd: 套接字
 * @param addr: 要连接的对方地址
 * @param wait_seconds: 等待超时秒数，如果为0表示正常模式
 * @return 成功（未超时）返回0，超时返回-1并且errno = ETIMEDOUT 其他错误返回-1并设置errno
 */
int connect_timeout(int fd, struct sockaddr_in *addr/*in*/, unsigned int wait_seconds);


/**
 * @brief 设置I/O为非阻塞模式
 * @param fd: 文件描符符
 * @return 成功返回0，失败返回-1并设置errno
 */
int set_io_to_nonblock(int fd);


/**
 * @brief 设置I/O为阻塞模式
 * @param fd: 文件描符符
 * @return 成功返回0，失败返回-1并设置errno
 */
int set_io_to_block_mode(int fd);



#ifdef __cplusplus
}
#endif

#endif /* _SYS_UTIL_H_ */
