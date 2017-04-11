
#include "common.h"
#include<iostream>
void printMsg(const msg_t *msg) {
	
	std::cout << "msg size:" << msg->size;

	switch (msg->type)
	{
		/*common*/
	case SEND:
		std::cout << "  type:SEND" << "  src:" << msg->src << "  dest:" << msg->dest << "  head:" << *(int*)msg->head << std::endl
			<< "    body:" << msg->body << std::endl;
		break;

			/*client to server*/
	case LOGIN:
		std::cout << "  type:LOGIN" << "  src:" << msg->src << "  dest:" << msg->dest << "  head:" << *(int*)msg->head << std::endl
			<< "    body:" << msg->body << std::endl;
		break;
	case REGISTER:
		std::cout << "  type:REGISTER" << "  src:" << msg->src << "  dest:" << msg->dest << "  head:" << *(int*)msg->head << std::endl
			 << std::endl;
		break;

			/*server to client*/
	case LOGIN_RES:
		std::cout << "  type:LOGIN_RES" << "  src:" << msg->src << "  dest:" << msg->dest << "  head:" << *(int*)msg->head << std::endl
			<< "    body:" << msg->body << std::endl;
		break;
	case REGISTER_RES:
		std::cout << "  type:REGISTER_RES" << "  src:" << msg->src << "  dest:" << msg->dest << "  head:" << *(int*)msg->head << std::endl
			<< std::endl;
		break; 
			
	case UPDATE_STUSTAS:
	{
		int onLineNums = 0;
		std::cout << "  type:UPDATE_STUSTAS" << "  src:" << msg->src << "  dest:" << msg->dest << "  head:" << (onLineNums=*(int*)msg->head) << std::endl
			<< std::endl;
		char *start =(char*) msg->body;
		for (int i = 0; i < onLineNums; ++i) {
			std::cout << *(int*)start << "     ";
			start += sizeof(4);
			std::cout << start << std::endl;
			start += 60;
		}
	}
		break;
	case SYSTEM:
		std::cout << "  type:SYSTEM" << "  src:" << msg->src << "  dest:" << msg->dest << "  head:" <<  *(int*)msg->head << std::endl
			<< std::endl;
		break;
	}

}