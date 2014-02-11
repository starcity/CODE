#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QUdpSocket>
#include <QHostAddress>
#include "Netinfo.h"
#include "NetObject.h"

class NetWork;
typedef std::map<QTcpSocket *,QHostAddress> CMapSockAddr;
typedef CMapSockAddr::iterator iter_CMapSockAddr;

class map_finder
{
public:
       map_finder(QHostAddress addr):_addr(addr){}
       bool operator ()(CMapSockAddr::value_type &pair)
       {
            return pair.second == _addr;
       }
private:
        QHostAddress _addr;
};

class Server:public QObject,public NetObject
{
    Q_OBJECT

public:
    Server(NetWork *parent);
    ~Server();

    int StarServer(int protocolType,QHostAddress addr,int port);
    int SendTcpData(QHostAddress addr,const char *data,int len);
    int SendUdpData(QHostAddress addr,int port,const char *data,int len);
    int SendData(const char *data,int len);

    void CloseConnect();

private slots:
    int RecvTcpData();
    int RecvUdpData();
    int OnConnected();
    int OnDisconnected();
    int ShowError(QAbstractSocket::SocketError);

public:
        int _type;

private:
    QTcpServer *_tcpServer;
    QUdpSocket *_udpServer;
    NetWork *_parent;
    CMapSockAddr _mapSockAddr;
};

#endif // SERVER_H
