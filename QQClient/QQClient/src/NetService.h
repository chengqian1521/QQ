#ifndef NETSERVICE_H
#define NETSERVICE_H

#include<QObject>
#include<QTcpSocket>
#include<functional>



#include "common.h"
#include "QQUser.h"
class QTcpSocket;


class NetService :public  QObject
{
    Q_OBJECT
public:
    static NetService* getInstance();
    void connectToServer();
    void disConnectFromServer();

	 QAbstractSocket::SocketState getTcpState();

	bool waitForConnected(int ms=3000);


    void registerOnErrorCallback(const std::function<void(QAbstractSocket::SocketError)>& onErrorCallback);
	inline void registerOnRecvNewMessageCallback(const std::function<void(int, const QString&)> &onRecvNewMessageCallback) {
		_onRecvNewMessageCallback = onRecvNewMessageCallback;
	}
	void registerOnRecvRegisterResCallback(const std::function<void(int)> &onRecvRegisterResCallback);
	inline void registerOnRecvLoginResultCallback(const std::function<void(int, const QString&)> &onRecvLoginResultCallback) {
		_onRecvLoginResultCallback = onRecvLoginResultCallback;
	}
	inline void registerOnRecvUpdateUsersStateCallback(const std::function<void(std::vector<QQUser>&)>& onRecvUpdateUsersStateCallback) {
		_onRecvUpdateUsersStateCallback = onRecvUpdateUsersStateCallback;
	}

    bool sendLoginMessage(int qq,const QString& pwd);
	bool sendNormalMessage(int srcQQ,int destQQ,const QString& content);
	
	/************************************************************************/
	/* 
		on ok return 0
		on err return -1
	*/
	/************************************************************************/
	int sendRegisterMessage(int qq,const QString& pwd,const QString& name);
	bool isConnectedOk();


private:
    NetService();
   
    void readNetConfig(QString& ip /*out*/,int& port /*out*/);
	int  writeN(char* data,int len);
	int  readN(char *data,int len);
	bool sendMessage(const msg_t* msg);


	void handleRegisterResult(const msg_t* msg);
	void handleLoginResult(const msg_t *msg);
	void handleUpdateAllUser(const msg_t *msg);
	void handleSendMessage(const msg_t *msg);


	void onQTimerTimeout();

private slots:
	void socketErrorCallback(QAbstractSocket::SocketError sockErr);
	void recvNewMessageFromServerCallback();


	

private:
    static NetService* s_netServcice;
    QTcpSocket       *_tcpSocket=nullptr;



    /*call back*/
    std::function<void(QAbstractSocket::SocketError)> _onErrorCallback=nullptr;
    std::function<void(int,const QString&)>           _onRecvNewMessageCallback=nullptr;
	std::function<void(int)>                          _onRecvRegisterResCallback = nullptr;
	std::function<void(int, const QString&)>          _onRecvLoginResultCallback = nullptr;
	std::function<void(std::vector<QQUser>&)>         _onRecvUpdateUsersStateCallback = nullptr;
	
	//std::function<void(int)>

};

#endif // NETSERVICE_H
