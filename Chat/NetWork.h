#ifndef NETWORK_H
#define NETWORK_H
#include "Server.h"
#include "TcpClient.h"
#include "UdpClient.h"
#include "ChatUi.h"
#include "NetObject.h"


typedef std::map<NetObject *,QHostAddress> CMapAddrObject;
typedef std::map<QString,QByteArray>  CMapAddrData;



class find_Object
{
public:
       find_Object(QHostAddress addr):_addr(addr){}
       bool operator ()(CMapAddrObject::value_type &pair)
       {
            return pair.second == _addr;
       }
private:
        QHostAddress _addr;
};


class QMyChat;
class NetWork :public QObject
{
    Q_OBJECT

public:
    NetWork(QMyChat * parent = NULL);
    ~NetWork();

public:
    int StartServer(int protocolType, QHostAddress addr,int port);
    int TcpConnect(QHostAddress addr,int port);
    int TcpShortConnect(QHostAddress addr,int port,const char *data,int len);
    int ClientUdp(QHostAddress addr,int port,const char *data,int len);

public:
    int TcpClientSendData(QHostAddress addr,const char *data,int len);
    int UdpClientSendData(QHostAddress addr,int port,const char *data,int len);
    int TcpServerSendData(QHostAddress addr,const char *data,int len);
    int UdpServerSendData(QHostAddress addr,const char *data,int len);
    int recvData(QHostAddress addr,int port,const char *data,int len);
    int NewConnect(QHostAddress addr);
    int CloseTcp(QHostAddress addr);

signals:
    void sendMesToMain(const QStringList str);

private slots:


private:
     QMyChat *_parent;
     Server *_server;
     CMapAddrObject _mapAddrObject;
     CMapAddrData   _mapAddrData;
};

#endif // NETWORK_H
