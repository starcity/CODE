#include "TcpClient.h"
#include "NetWork.h"
#include "Chat_strcutinfo.h"


TcpClient::TcpClient(NetWork *parent)
{
    _client = new QTcpSocket(this);
    _parent = parent;
    _connectTimes = 0;
    _isconnect = false;
    _type = TCPCLIENT;
}

TcpClient::~TcpClient()
{
    if(_client != NULL)
    {
        delete _client;
        _client = NULL;
    }
    _parent = NULL;
}

int TcpClient::ShortConnect(QHostAddress addr,const int port,const char *data,int len)
{
    QTcpSocket tmpClient(this);
    tmpClient.connectToHost(addr,port);
    if(tmpClient.waitForConnected() == false)
    {
        return -300;
    }

    int nRet = 0;
    while(nRet < len) //一次性发送会出现丢包现象，待解决后，重写
    {
        if(len - nRet > 8000)
            nRet += tmpClient.write(data + nRet,8000);
        else
            nRet += tmpClient.write(data + nRet,len -nRet);
        if(tmpClient.waitForBytesWritten(-1) == false)
        {
            return -400;
        }
    }
    tmpClient.close();
    return nRet;
}

int TcpClient::Connect(QHostAddress addr,const int port)
{
    if(_client == NULL)
         _client = new QTcpSocket(this);

    connect(_client,SIGNAL(readyRead()),this,SLOT(RecvData()));
    connect(_client,SIGNAL(connected()),this,SLOT(OnConnect()));
    connect(_client,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(displayError(QAbstractSocket::SocketError)));

    _addr = addr;
    _port = port;
    _client->connectToHost(addr,port);
    _connectTimes++;
}

int TcpClient::RecvData(char *data,int len)
{
    if(_client->bytesAvailable() > 5){
        char data[_client->bytesAvailable()];
        int nRet = _client->read(data,sizeof(data));
//        if((data[0] == (char)0xFF && data[1] == (char)0xFF) && \
//                (data[nRet -2] == (char)0xFF && data[nRet -1] == (char)0xFF))
        if(nRet > 0)
        {
            _parent->recvData(_client->peerAddress(),_client->peerPort(),data,nRet);
        }
    }
}

int TcpClient::SendData(const char *data,int len)
{
    if(_isconnect){
        return _client->write(data,len);
    }
    else {
        QStringList str;
        str.push_back(QString("Fortune Client"));
        str.push_back(QString("Not Connect"));
////        _parent->ShowTcpMsg(str);

        return -1;
    }
    return 0;
}

void TcpClient::displayError(QAbstractSocket::SocketError socketError)
{
    QStringList str;
    switch (socketError)
    {
    case QAbstractSocket::RemoteHostClosedError:
        str.push_back(QString("Fortune Client"));
        str.push_back(QString("The remote host closed the connection."\
                              "Note that the client socket will be closed"\
                              "after the remote close notification has been sent."));
        break;
    case QAbstractSocket::HostNotFoundError:
        str.push_back(QString("Fortune Client"));
        str.push_back(QString("The host was not found. Please check the "\
                              "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        str.push_back(QString("Fortune Client"));
        str.push_back(QString("The connection was refused by the peer. "\
                              "Make sure the fortune server is running, "\
                              "and check that the host name and port "\
                              "settings are correct."));
        break;
    default:
        str.push_back(QString("Fortune Client"));
        str.push_back(QString("The following error occurred:" + _client->errorString()));
        break;
    }
//    _parent->ShowTcpMsg(str);

    _client->close();
    _isconnect = false;

    if(_connectTimes < 5){
        QTime t;
        t.start();
        while(t.elapsed() < 1000*(_connectTimes*3))
            QCoreApplication::processEvents();
       Connect(_addr,_port);
    }
}

void TcpClient::OnConnect()
{
    _isconnect = true;
}

void TcpClient::CloseConnect()
{
    _client->close();
}


