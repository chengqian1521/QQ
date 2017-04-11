#ifndef WORK_H_
#define WORK_H_




#include "unordered_map"

#include "QQUser.h"


class msg_t;

class QQUserDao;

class QQServer
{
public:
    QQServer(int port);
    ~QQServer();

    QQUserDao* _qqUserDao;

    int run();
	
private:



	void sendmsg(const struct msg_t *msg, ssize_t msglen); //处理send消息

    void socket_fd_close_before(int fd);
	void user_logout(int st); //client socket连接断开


    void addOnlineUser(int socket_fd,QQUser& qqUser);
    void broadcastUserState(); //向socket_client[]数组中所有client广播用户状态消息

    int  recvCompletMessage(int st);

    /**
     * @brief sendCompletMessage
     * @param target_fd
     * @param msg
     * @return on ok return 0,on err return -1 and set errno
     */
    int  sendCompletMessage(int target_fd,const msg_t* msg);

    /**
     * @brief handle_regster_message
     * @param msg
     * @return on ok return 0,qq alread exist return 1,on err return -1
     */
    int handleRegsterMessage(int sender_fd,const msg_t* msg);

    void handleLoginMessage(int src_st,const msg_t* msg); //处理login消息

    void handleSendMessage(int st,const msg_t* msg);

	int listen_st;

    /*qq to OnlineQQUserInfo Map*/
    std::unordered_map<int,QQUser> _qq_to_qqUser;

    /*fd to qq map*/
    std::unordered_map<int,int> _fd_to_qq;
};
#endif /* WORK_H_ */
