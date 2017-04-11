#include "NetService.h"
#include<QTcpSocket>
#include<QMessageBox>
#include<QDomDocument>
#include<QHostAddress>
#include<QFile>
#include<QMessageBox>
#include<QDomDocument>
#include<QDomNodeList>
#include<QRegExp>
#include "QQUser.h"
#include <QTimer>
#include "common.h"

NetService* NetService::s_netServcice(nullptr);

NetService::NetService()
{
	_tcpSocket = new QTcpSocket(this);
	void(QAbstractSocket::*errorSig)(QAbstractSocket::SocketError) = &QAbstractSocket::error;
	connect(_tcpSocket, errorSig, std::bind(&NetService::socketErrorCallback, this, std::placeholders::_1));
    connect(_tcpSocket, &QTcpSocket::readyRead,std::bind(&NetService::recvNewMessageFromServerCallback, this));//设置socket读消息的槽函数
	
	connect(_tcpSocket, &QTcpSocket::connected, [&]() {
		qDebug() << "conneected";
	});
  
	QTimer* timer = new QTimer(this);
	connect(timer, &QTimer::timeout, std::bind(&NetService::onQTimerTimeout, this));
	timer->setInterval(2000);
	timer->start();

    connectToServer();	
}

NetService *NetService::getInstance()
{
    if(s_netServcice)
        return s_netServcice;
    return s_netServcice=new NetService();
}

void NetService::connectToServer()
{
	QString ip;
	int port;
	readNetConfig(ip, port);
    _tcpSocket->connectToHost(ip, port);
}

QAbstractSocket::SocketState NetService::getTcpState() {
	return _tcpSocket->state();
}

void NetService::disConnectFromServer()
{
   if(_tcpSocket->isOpen()){
       if(_tcpSocket->state()==QTcpSocket::ConnectedState){
           _tcpSocket->close();
       }
   }
}

void NetService::onQTimerTimeout() {


}

bool NetService::waitForConnected(int ms)
{
	return _tcpSocket->waitForConnected(ms);
}


void NetService::registerOnErrorCallback(const std::function<void(QAbstractSocket::SocketError)> &onErrorCallback)
{
    this->_onErrorCallback=onErrorCallback;
}



void NetService::registerOnRecvRegisterResCallback(const std::function<void(int)> &onRecvRegisterResCallback) {
	_onRecvRegisterResCallback = onRecvRegisterResCallback;
}
bool NetService::isConnectedOk() {	
	return _tcpSocket->isOpen() && _tcpSocket->state() == QTcpSocket::ConnectedState;
}


bool NetService::sendLoginMessage(int qq, const QString& pwd)
{
    
	int msg_len = MESSAGE_MAIN_LEN + 64;
	msg_t *loginMsg=(msg_t *)new char[msg_len];
	memset(loginMsg, 0, msg_len);
	loginMsg->size = msg_len;
	loginMsg->type = LOGIN;
	loginMsg->src = qq;
	QByteArray pwdU8Array = pwd.toUtf8();
	memmove(loginMsg->body, pwdU8Array.data(), pwdU8Array.size());

	bool ret = false;
	if (!isConnectedOk()) {
		ret = false;
		goto END;
	}

	ret=sendMessage(loginMsg);

END:
	delete loginMsg;
	return ret;
}
bool NetService::sendNormalMessage(int srcQQ,int destQQ, const QString& content) {

	 QByteArray u8ByteArray = content.toUtf8();

	 int msg_len = MESSAGE_MAIN_LEN + u8ByteArray.size() + 5;
	 msg_t *msg = (msg_t*)new char[msg_len];
	 memset(msg, 0, msg_len);

	 msg->size = msg_len;
	 msg->type = SEND;
	 msg->dest = destQQ;
	 msg->src = srcQQ;
	 *(int*)msg->head = u8ByteArray.size();
	 memmove(msg->body,u8ByteArray.data(),u8ByteArray.size());
	 bool ret = sendMessage(msg);

	 delete msg;
	 return ret;
}

void NetService::readNetConfig(QString &ip, int &port)
{
    QDomDocument doc("ip-config");
    QFile file("ip-config.xml");
    if (!file.open(QIODevice::ReadOnly)){
        QMessageBox::information(nullptr,"file open error",file.errorString());
        exit(-1);
    }
    if (!doc.setContent(&file))
    {
        QMessageBox::information(nullptr,"QDomDocument set error","QDomDocument set error");
        file.close();
        exit(-1);
    }
    file.close();


    QDomNodeList ipDoc=doc.elementsByTagName("ip");
    if(ipDoc.isEmpty()){
        QMessageBox::information(nullptr,"ip config err","ip config err");
        exit(-1);
    }
    ip=ipDoc.at(0).toElement().text();



    QRegExp reg("^(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|[1-9])\\."
                "(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|\\d)\\."
                "(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|\\d)\\."
                "(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|\\d)$");
    if(!reg.exactMatch(ip)){
        QMessageBox::information(nullptr,"ip config fommat err ","ip fommat err");
        exit(-1);
    }

    QDomNodeList portDoc=doc.elementsByTagName("port");
    if(portDoc.isEmpty()){
        QMessageBox::information(nullptr,"config err","port config not found");
        exit(-1);
    }

    bool convertToIntOk=false;
    port=portDoc.at(0).toElement().text().toUInt(&convertToIntOk);
    if(!convertToIntOk){
        QMessageBox::information(nullptr,"config err","port must is number");
        exit(-1);
    }
    if(port<0 || port >=65535){
        QMessageBox::information(nullptr,"config err","port must rage 0~65535");
        exit(-1);
    }

}

int NetService::sendRegisterMessage(int qq, const QString& pwd, const QString& name) {
	
	

	QString body = pwd + ";" + name;
	int msg_len = MESSAGE_MAIN_LEN + 128;
	msg_t *registMsg = (msg_t *)new char[msg_len];
	memset(registMsg, 0, msg_len);
	registMsg->size = msg_len;
	registMsg->src = qq;
	registMsg->type = REGISTER;
	QByteArray byteArray = body.toUtf8();
	memmove(registMsg->body, byteArray.data(), byteArray.size());
	
	bool ret=sendMessage(registMsg);
	delete registMsg;
	return ret;
}

bool NetService::sendMessage(const msg_t* msg) {
	char *start = (char *)&msg;
	if (writeN((char*)msg, msg->size) == -1) {
		return false;
	}
	return true;
}

int  NetService::writeN(char* data, int len) {
	if (! data|| len<=0)
		return -1;
	int left = len;
	int ret = 0;
	while (left > 0) {
		ret = _tcpSocket->write(data, left);
		if (ret == -1) {
			return -1;
		}

		data += ret;
		left -= ret;
	}

	if (_tcpSocket->flush())
		return 0;
	return -1;


}
int  NetService::readN(char *data, int len) {
	if (!data || len <= 0)
		return -1;
	int left = len;
	int ret = 0;
	while (left > 0) {
		ret = _tcpSocket->read(data, left);
		if (ret == -1) {
			return -1;
		}

		data += ret;
		left -= ret;
	}

	
	return 0;
	
}
void NetService::recvNewMessageFromServerCallback()//socket接收到来自server端的消息时调用的槽函数
{
	
	int msg_len = 0;
	if (readN((char*)&msg_len, 4) == -1) {
		QMessageBox::information(nullptr, "net error", "readN error");
		exit(-1);
	}

	msg_t* buf = (msg_t*)new char[msg_len];
	buf->size = msg_len;
	
	if (readN((char*)buf + 4, msg_len - 4) == -1) {
		QMessageBox::information(nullptr, "net error", "readN error");
		exit(-1);
	}

	switch (buf->type)
	{
	case LOGIN_RES:
		handleLoginResult(buf);
		break;
	case REGISTER_RES:
		handleRegisterResult(buf);
		break;
	case UPDATE_STUSTAS:
		handleUpdateAllUser(buf);
		break;
	case SEND:
		handleSendMessage(buf);
		break;
	case SYSTEM:
		break;
	default:
		break;
	}
	delete buf;
    
}
void NetService::handleRegisterResult(const  msg_t* msg) {
	
	if (msg->head[0] == -1) {
		QMessageBox::information(nullptr, "net error", "服务器端错误");
		exit(-1);
	}

	if (_onRecvRegisterResCallback)
			_onRecvRegisterResCallback(msg->head[0]);
		
}

void NetService::handleLoginResult(const msg_t *msg) {
	
	QString thisName(msg->body);
	
	if (_onRecvLoginResultCallback) {
		_onRecvLoginResultCallback(msg->head[0], thisName);
	}

}
void NetService::handleUpdateAllUser(const msg_t *msg) {

	std::vector<QQUser> onLineUsers;

	char* start = (char*)msg->body;

	int onLineUserCount = *(int*)msg->head;
	int qq;
	QString name;
	for (int i = 0; i < onLineUserCount; ++i) {

		qq = *(int*)start;
		start += 4;

		name = QString::fromUtf8(start);
		start += 60;
		onLineUsers.push_back({ qq,name });
	}

	if (_onRecvUpdateUsersStateCallback)
		  _onRecvUpdateUsersStateCallback(onLineUsers);

}
void NetService::handleSendMessage(const msg_t *msg) {
	
	QString content(msg->body);
	int src = msg->src;

	if (_onRecvNewMessageCallback) 
		 _onRecvNewMessageCallback(src,content);
	
}
void NetService::socketErrorCallback(QAbstractSocket::SocketError sockErr)//socket出错时调用的槽函数
{
    switch (sockErr)
    {
    case QAbstractSocket::SocketTimeoutError                    :
        //传包超时
    QMessageBox::information(nullptr,"NetService ..socket Error",_tcpSocket->errorString());

        break;
    case QAbstractSocket::RemoteHostClosedError:
		QMessageBox::information(nullptr,"net err","服务器拒绝了您的连接，您的账号可能在另外一个设备上登录");
		//返回登陆界面

		if(_onErrorCallback) 
			_onErrorCallback(sockErr);

		break;
    case QAbstractSocket::ConnectionRefusedError                :

    case QAbstractSocket::HostNotFoundError                     :
    case QAbstractSocket::SocketAccessError                     :
    case QAbstractSocket::SocketResourceError                   :

    case QAbstractSocket::DatagramTooLargeError                 :
    case QAbstractSocket::NetworkError                          :
    case QAbstractSocket::AddressInUseError                     :
    case QAbstractSocket::SocketAddressNotAvailableError        :
    case QAbstractSocket::UnsupportedSocketOperationError       :
    case QAbstractSocket::ProxyAuthenticationRequiredError      :
    case QAbstractSocket::SslHandshakeFailedError               :
    case QAbstractSocket::UnfinishedSocketOperationError        :
    case QAbstractSocket::ProxyConnectionRefusedError           :
    case QAbstractSocket::ProxyConnectionClosedError            :
    case QAbstractSocket::ProxyConnectionTimeoutError           :
    case QAbstractSocket::ProxyNotFoundError                    :
    case QAbstractSocket::ProxyProtocolError                    :
    case QAbstractSocket::OperationError                        :
    case QAbstractSocket::SslInternalError                      :
    case QAbstractSocket::SslInvalidUserDataError               :
    case QAbstractSocket::TemporaryError                        :
    case QAbstractSocket::UnknownSocketError                    :
        QMessageBox::information(nullptr,"NetService  socket Error",_tcpSocket->errorString());
		exit(0);
        break;
    }
}


