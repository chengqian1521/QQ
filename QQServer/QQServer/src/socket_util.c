#include "socket_util.h"



ssize_t read_fix_n(int fd, void *buf, size_t count)
{
	//size_t 在x64下为 unsigned long 类型， 在x86下为 unsigned int 类型
	size_t nleft = count; //将count接过来 ，个数
    size_t nread;
	char *bufp = (char*) buf; //将空指针类型转换为char类型指针。
	while (nleft > 0)
	{
		/*ssize_t read(int fd, void *buf, size_t count);
		 * 从文件描述符fd中读取count字节存到buf中
		 * 返回读取字节数的个数。
		 * */
		if ((nread = read(fd, bufp, nleft)) < 0)
		{
			/*
			 * 如果是被信号中断的继续读
			 * */
			if (errno == EINTR)
				continue;
			return -1;
		}
		/*
		 * 如果输入的读取个数为0，那么返回的读取个数为0
		 * 不执行任何操作。
		 * nleft为剩余的需要读取的字节个数。
		 * 如果为0，说明读到文件尾，
         * */
		else if (nread == 0)
			return count - nleft;
		bufp += nread; //将字符指针向前推进已成功读取字符数的大小单位。
		nleft -= nread; //剩余的个数减去已经成功读取的字节数。
	}
	return count;
}


ssize_t write_fix_n(int fd, const void *buf, size_t count)
{
	size_t nleft = count; //剩余的需要写入的字节数。
	ssize_t nwritten; //成功写入的字节数。
	char *bufp = (char*) buf; //将缓冲的指针强制转换为字符类型的指针。
	/*
	 * 如果剩余需要写入的字节数大于0则继续
	 * */
	while (nleft > 0)
	{
		/*
		 * ssize_t write(int fd, const void *buf, size_t count);
		 * fd为需要写入的文件描述符，buf为字符缓存区，count为需要写入的字节数。
		 *
		 * */
		if ((nwritten = write(fd, bufp, nleft)) < 0)
		{
			/*
			 * 如果是被信号中断的继续读
			 * */
			if (errno == EINTR)
				continue;
			return -1;
		} else if (nwritten == 0)
			continue;
		//字符指针推移已经写入成功大小的字节数。
		bufp += nwritten;
		//剩余的字节数。
		nleft -= nwritten;
	}
	return count;
}

ssize_t read_line(int sockfd, char *buf, size_t maxline)
{
    ssize_t ret;
    size_t nread;
	char *bufp = buf;
    size_t nleft = maxline;
    int alreadyRead=0;
	while (1)
	{
        ret = recv_peek(sockfd, bufp, nleft);
		if (ret < 0)
            return -1;
		else if (ret == 0)
			return ret;

		nread = ret;
        size_t i;
		for (i=0; i<nread; i++)
		{
			if (bufp[i] == '\n')
			{
                ret = read_fix_n(sockfd, bufp, i+1);

                if ((size_t)ret != i+1)
					exit(EXIT_FAILURE);
                alreadyRead+=ret;
                return alreadyRead;
			}
		}

		if (nread > nleft)
            return -2;//该行超出缓冲区的长度

		nleft -= nread;
        ret = read_fix_n(sockfd, bufp, nread);
        if (ret !=(ssize_t) nread)
			exit(EXIT_FAILURE);
        alreadyRead+=ret;
		bufp += nread;
	}

	return -1;
}



ssize_t recv_peek(int sockfd, char *buf, size_t len)
{
	while (1)
	{
		/*
		 * ssize_t recv(int sockfd, void *buf, size_t len, int flags);
		 * sockfd 套接字
		 * len 需要读取的长度
		 * MSG_PEEK只从队列中查看，但不取出。
         * 返回接受到的字节的长度，失败返回-1，收到关闭信号返回0；
         **/
		int ret = recv(sockfd, buf, len, MSG_PEEK);

        /*
		 * 如果被信号中断了，继续
		 * */
		if (ret == -1 && errno == EINTR)
			continue;
		return ret;
	}
}



int read_timeout_check(int fd, unsigned int wait_seconds)
{
	int ret = 0;
	if (wait_seconds > 0)
	{
		fd_set read_fdset;
		struct timeval timeout;

		FD_ZERO(&read_fdset);
		FD_SET(fd, &read_fdset);

		timeout.tv_sec = wait_seconds;
		timeout.tv_usec = 0;
		
		//select返回值三态
		//1 若timeout时间到（超时），没有检测到读事件 ret返回=0
		//2 若ret返回<0 &&  errno == EINTR 说明select的过程中被别的信号中断（可中断睡眠原理）
		//2-1 若返回-1，select出错
		//3 若ret返回值>0 表示有read事件发生，返回事件发生的个数
		
		do
		{
			ret = select(fd + 1, &read_fdset, NULL, NULL, &timeout);
		} while (ret < 0 && errno == EINTR);

		if (ret == 0)
		{
			ret = -1;
			errno = ETIMEDOUT;
		}
		else if (ret == 1)
			ret = 0;
	}

	return ret;
}


int write_timeout_check(int fd, unsigned int wait_seconds)
{
	int ret = 0;
	if (wait_seconds > 0)
	{
		fd_set write_fdset;
		struct timeval timeout;

		FD_ZERO(&write_fdset);
		FD_SET(fd, &write_fdset);

		timeout.tv_sec = wait_seconds;
		timeout.tv_usec = 0;
		do
		{
			ret = select(fd + 1, NULL, &write_fdset, NULL, &timeout);
		} while (ret < 0 && errno == EINTR);

		if (ret == 0)
		{
			ret = -1;
			errno = ETIMEDOUT;
		}
		else if (ret == 1)
			ret = 0;
	}

	return ret;
}

int accept_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds)
{
	int ret;
	socklen_t addrlen = sizeof(struct sockaddr_in);

	if (wait_seconds > 0)
	{
		fd_set accept_fdset;
		struct timeval timeout;
		FD_ZERO(&accept_fdset);
		FD_SET(fd, &accept_fdset);
		timeout.tv_sec = wait_seconds;
		timeout.tv_usec = 0;
		do
		{
			ret = select(fd + 1, &accept_fdset, NULL, NULL, &timeout);
		} while (ret < 0 && errno == EINTR);
		if (ret == -1)
			return -1;
		else if (ret == 0)
		{
			errno = ETIMEDOUT;
			return -1;
		}
	}

	//一但检测出 有select事件发生，表示对等方完成了三次握手，客户端有新连接建立
	//此时再调用accept将不会堵塞
    if (addr)
		ret = accept(fd, (struct sockaddr*)addr, &addrlen); //返回已连接套接字
	else
		ret = accept(fd, NULL, NULL);
	if (ret == -1)
		ERR_EXIT("accept");

	return ret;
}



int connect_timeout(int fd, struct sockaddr_in *addr,
        unsigned int wait_seconds)
{
    int ret;
    //获取socket结构体的大小。
    socklen_t addrlen = sizeof(struct sockaddr_in);
    //如果传入的等待时间大于0就取消socket的阻塞状态，0则不执行。
    if (wait_seconds > 0)
        set_io_to_nonblock(fd);
    //链接
    /*
     * int connect(int sockfd, const struct sockaddr *addr,socklen_t addrlen);
     **/
    ret = connect(fd, (struct sockaddr*) addr, addrlen);
    //EINPROGRESS 正在处理
    if (ret < 0 && errno == EINPROGRESS)
    {
        /*
         * void FD_CLR(int fd, fd_set *set);
         * int  FD_ISSET(int fd, fd_set *set);
         * void FD_SET(int fd, fd_set *set);
         * void FD_ZERO(fd_set *set);
         * */
        //设置监听集合
        fd_set connect_fdset;
        struct timeval timeout;
        //初始化集合
        FD_ZERO(&connect_fdset);
        //把fd 文件描述符的socket加入监听集合
        FD_SET(fd, &connect_fdset);
        /*
         * struct timeval {
         *     long    tv_sec;         // seconds       秒
         *     long    tv_usec;        // microseconds  微妙
         *     };
         * */
        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;
        do
        {
            // 一但连接建立,则套接字就可写  所以connect_fdset放在了写集合中
            ret = select(fd + 1, NULL, &connect_fdset, NULL, &timeout);
        } while (ret < 0 && errno == EINTR);
        if (ret == 0)
        {
            /**
             * #define ETIMEDOUT    110     // Connection timed out
             *  Tcp是面向连接的。在程序中表现为，当tcp检测到对端socket不再可
             *  用时(不能发出探测包，或探测包没有收到ACK的响应包)，select会
             *  返回socket可读，并且在recv时返回-1，同时置上errno为ETIMEDOUT。
             **/
            ret = -1;
            errno = ETIMEDOUT;
        } else if (ret < 0)
            return -1;
        else if (ret == 1)
        {
            /* ret返回为1(表示套接字可写)，可能有两种情况，一种是连接建立成功，一种是套接字产生错误，*/
            /* 此时错误信息不会保存至errno变量中，因此，需要调用getsockopt来获取。 */
            int err;
            socklen_t socklen = sizeof(err);
            //获取socket的状态
            int sockoptret = getsockopt(fd, SOL_SOCKET, SO_ERROR, &err,
                    &socklen);
            if (sockoptret == -1)
            {
                return -1;
            }
            if (err == 0)
            {
                ret = 0;
            }
            else
            {
                errno = err;
                ret = -1;
            }
        }
    }
    if (wait_seconds > 0)
    {
        set_io_to_block_mode(fd);
    }
    return ret;
}


int set_io_to_nonblock(int fd)
{
	int ret = 0;
	/*
	 * int fcntl(int fd, int cmd, ... \* arg \*)
	 获取文件或者修改文件状态
	 F_GETLK 取得文件锁定的状态。
	 返回值 成功则返回0，若有错误则返回-1，错误原因存于errno.
	 */
	int flags = fcntl(fd, F_GETFL);
	if (flags == -1)
	{
		ret = flags;
		printf("func activate_nonblock() err:%d", ret);
		return ret;
	}
	/*
	 *  按位或，加上没有锁的状态
	 * */
	flags |= O_NONBLOCK;
	/*
	 * . F_SETFL ：设置文件状态标志。
	 其中O_RDONLY， O_WRONLY， O_RDWR， O_CREAT， O_EXCL， O_NOCTTY 和 O_TRUNC不受影响，
	 可以更改的标志有 O_APPEND，O_ASYNC， O_DIRECT， O_NOATIME 和 O_NONBLOCK。
	 * */
	ret = fcntl(fd, F_SETFL, flags);
	if (ret == -1)
	{
		printf("func activate_nonblock() err:%d", ret);
		return ret;
	}
	return ret;
}

int set_io_to_block_mode(int fd)
{
	int ret = 0;
	/*
	 * int fcntl(int fd, int cmd, ... \* arg \*)
	 获取文件或者修改文件状态
	 F_GETLK 取得文件锁定的状态。
	 返回值 成功则返回0，若有错误则返回-1，错误原因存于errno.
	 */
	int flags = fcntl(fd, F_GETFL);
	if (flags == -1)
	{
		ret = flags;
		printf("func deactivate_nonblock() err:%d", ret);
		return ret;
	}

	/*
	 * 按位与， NONBLOCK的按位反 并上状态
	 * */
	flags &= ~O_NONBLOCK;
	/*
	 * . F_SETFL ：设置文件状态标志。
	 其中O_RDONLY， O_WRONLY， O_RDWR， O_CREAT， O_EXCL， O_NOCTTY 和 O_TRUNC不受影响，
	 可以更改的标志有 O_APPEND，O_ASYNC， O_DIRECT， O_NOATIME 和 O_NONBLOCK。
	 * */
	ret = fcntl(fd, F_SETFL, flags);
	if (ret == -1)
	{
		printf("func deactivate_nonblock() err:%d", ret);
		return ret;
	}
	return ret;
}





