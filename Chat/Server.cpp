#include "Server.h"
#include "NetWork.h"
#include "Chat_strcutinfo.h"



 Server::Server(NetWork *parent)
 {
     _parent = parent;
     _mapSockAddr.clear();
 }

 Server::~Server()
 {
     if(_tcpServer != NULL)
     {
         delete _tcpServer;
         _tcpServer = NULL;
     }
     if(_udpServer != NULL)
     {
         delete _udpServer;
         _udpServer = NULL;
     }
     _parent = NULL;
 }

int Server::StarServer(int protocolType,QHostAddress addr,int port)
{
    if(protocolType == 0)
    {
        _tcpServer = new QTcpServer(this);
        _tcpServer->listen(addr,port);
        connect(_tcpServer,SIGNAL(newConnection()),this,SLOT(OnConnected()));
        connect(_tcpServer,SIGNAL(acceptError(QAbstractSocket::SocketError)),this,SLOT(ShowError(QAbstractSocket::SocketError)));
    }
    else if(protocolType == 1)
    {
        _udpServer = new QUdpSocket(this);
        _udpServer->bind(port);
        connect(_udpServer,SIGNAL(readyRead()),this,SLOT(RecvUdpData()));
    }
}

int Server::SendTcpData(QHostAddress addr,const char *data,int len)
{
    iter_CMapSockAddr iter = std::find_if(_mapSockAddr.begin(),_mapSockAddr.end(),map_finder(addr));
    if(iter == _mapSockAddr.end())
    {
        return -10;
    }
    if(iter->first->state() == QAbstractSocket::ConnectedState)
         return iter->first->write(data,len);
}

int Server::SendUdpData(QHostAddress addr,int port,const char *data,int len)
{
    return _udpServer->writeDatagram(data,len,addr,port);
}

int Server::RecvTcpData()
{
    for(iter_CMapSockAddr iter = _mapSockAddr.begin();iter !=_mapSockAddr.end();iter++)
    {
        if(iter->first->bytesAvailable() > 5)
        {
            char data[iter->first->bytesAvailable()];

            int nRet = iter->first->read(data,sizeof(data));
//            if((data[0] == (char)0xFF && data[1] == (char)0xFF) && \
//                    (data[nRet -2] == (char)0xFF && data[nRet -1] == (char)0xFF))
            if(nRet > 0)
            {
                _parent->recvData(iter->first->peerAddress(),iter->first->peerPort(),data,nRet);
            }
        }
    }
}

int Server::RecvUdpData()
{
    while (_udpServer->pendingDatagramSize() > 5) {
        QByteArray datagram;
        datagram.resize(_udpServer->pendingDatagramSize());

        QHostAddress addr;
        quint16 port;
        _udpServer->readDatagram(datagram.data(), datagram.size(),&addr,&port);
//        if((datagram[0] == (char)0xFF && datagram[1] == (char)0xFF) && \
//                (datagram[datagram.size()-2] == (char)0xFF && datagram[datagram.size() -1] == (char)0xFF))
        if(datagram.length() > 0)
        {
            _parent->recvData(addr,port,datagram.data(),datagram.size());
        }
    }
}

int Server::OnConnected()
{
    QTcpSocket *client = _tcpServer->nextPendingConnection();

    connect(client,SIGNAL(readyRead()),this,SLOT(RecvTcpData()));
    connect(client,SIGNAL(disconnected()),this,SLOT(OnDisconnected()));
    connect(client,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(ShowError(QAbstractSocket::SocketError)));

    _mapSockAddr.insert(std::make_pair(client,client->peerAddress()));
    _parent->NewConnect(client->peerAddress());
}

int Server::ShowError(QAbstractSocket::SocketError error)
{

}

int Server::SendData(const char *data,int len)
{

}

void Server::CloseConnect()
{

}

int Server::OnDisconnected()
{
    for(CMapSockAddr::iterator iter = _mapSockAddr.begin();iter != _mapSockAddr.end();iter++)
    {
        if(iter->first->state() != QAbstractSocket::ConnectedState)
        {
            _mapSockAddr.erase(iter);
            break;
        }
    }
}

