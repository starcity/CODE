#ifndef TCPCLIENT_H
#define TCPCLIENT_H
#include <QTcpSocket>
#include "Netinfo.h"
#include <QHostAddress>
#include "NetObject.h"

class NetWork;

class TcpClient:public QObject,public NetObject
{
    Q_OBJECT

public:
    TcpClient(NetWork *parent);
    ~TcpClient();

    int SendData(const char *data,int len);
    int ShortConnect(QHostAddress addr,const int port,const char *data,int len);
    int Connect(QHostAddress addr,const int port);
    void CloseConnect();

private:
    int RecvData(char *data,int len);

signals:

private slots:
        void displayError(QAbstractSocket::SocketError socketError);
        void OnConnect();

public:
        int _type;

private:
    QTcpSocket *_client;
    NetWork *_parent;

    QHostAddress _addr;
    int _port;
    int _connectTimes;
    bool _isconnect;
};

#endif // TCPCLIENT_H
