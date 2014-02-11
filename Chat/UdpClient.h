#ifndef UDPCLIENT_H
#define UDPCLIENT_H
#include <QUdpSocket>
#include "NetWork.h"
#include "Netinfo.h"

class NetWork;

class UdpClient:public QObject
{
    Q_OBJECT

public:
    UdpClient(NetWork *parent);
    ~UdpClient();

    int SendData(QHostAddress addr,int port,const char *data,int len);

signals:

private slots:
    int RecvUdpData();
    int ShowError(QAbstractSocket::SocketError);

public:
        int _type;

private:
    QUdpSocket *_udpClient;
    NetWork *_parent;
};

#endif // UDPCLIENT_H
