#include "UdpClient.h"


UdpClient::UdpClient(NetWork *parent)
{
    _udpClient = new QUdpSocket(this);
    _parent = parent;
    _type = UDPCLIENT;
}

UdpClient::~UdpClient()
{
    if(_udpClient != NULL)
    {
        delete _udpClient;
        _udpClient = NULL;
    }
    _parent = NULL;
}

int UdpClient::SendData(QHostAddress addr,int port,const char *data,int len)
{
    return _udpClient->writeDatagram(data,len,addr,port);
}

int UdpClient::RecvUdpData()
{
    while (_udpClient->pendingDatagramSize() > 5) {
        QByteArray datagram;
        datagram.resize(_udpClient->pendingDatagramSize());

        _udpClient->readDatagram(datagram.data(), datagram.size());
//        if((datagram[0] == (char)0xFF && datagram[1] == (char)0xFF) && \
//                (datagram[datagram.size()-2] == (char)0xFF && datagram[datagram.size() -1] == (char)0xFF))
        if(datagram.length() > 0)
        {
            _parent->recvData(_udpClient->peerAddress(),_udpClient->peerPort(),datagram.data(),datagram.size());
        }
    }
}

int UdpClient::ShowError(QAbstractSocket::SocketError)
{

}
