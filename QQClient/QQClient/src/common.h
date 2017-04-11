#ifndef COMMON_H
#define COMMON_H
#define QQDEBUG


#define netService (NetService::getInstance())
#pragma execution_character_set("utf-8")
#define FriendItemFixedHeight  50


#define HEAD_LEN 4
#define BODYBUF 1024
#define MESSAGE_MAIN_LEN ((int)((msg_t*)0)->body)
#define MSGBODYBUF 64*256
enum MessageType {
	/*common*/
	SEND,


	/*client to server*/

	LOGIN,
	REGISTER,

	/*server to client*/
	LOGIN_RES,
	REGISTER_RES,
	UPDATE_STUSTAS,
	SYSTEM,
};


struct msg_t
{
	int size; //接下来包的大小
	MessageType type;
	int src;
	int dest;
	char head[HEAD_LEN];
	char body[BODYBUF];
};

#endif // COMMON_H
