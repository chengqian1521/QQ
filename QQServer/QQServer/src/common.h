#ifndef COMMON_H
#define COMMON_H

#include "util.h"
#define QQDEBUG

#define MaxOnlineCount  200
#define MaxConnectCount 1024

#define HEAD_LEN 4
#define BODYBUF  0
#define MESSAGE_MAIN_LEN static_cast<int>((char*)((msg_t*)0)->body-(char*)0)

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

void printMsg(const msg_t *msg);

#endif // COMMON_H
