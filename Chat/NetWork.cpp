#include "NetWork.h"
#include <QHostAddress>
#include <QMessageBox>
#include <unistd.h>
#include "Netinfo.h"
#include "Chat_strcutinfo.h"


NetWork::NetWork(QMyChat *parent)
{
    _parent = parent;
    _server = NULL;
    _mapAddrObject.clear();
    _mapAddrData.clear();
}

NetWork::~NetWork()
{
    for(CMapAddrObject::iterator iter = _mapAddrObject.begin();iter !=_mapAddrObject.end();iter++)
    {
        delete iter->first;
    }
    if(_server == NULL)
        delete _server;
}

int NetWork::StartServer(int protocolType, QHostAddress addr,int port)
{
    _server = new Server(this);
    _server->StarServer(protocolType,addr,port);
}

int NetWork::TcpConnect(QHostAddress addr,int port)
{
    TcpClient *tcpclient =new TcpClient(this);
    tcpclient->Connect(addr,port);
    _mapAddrObject.insert(std::make_pair(tcpclient,addr));
}

int NetWork::TcpShortConnect(QHostAddress addr,int port,const char *data,int len)
{
    char sendData[len +2];
    memset(sendData,0xFF,sizeof(sendData));
    memcpy(sendData+2,data,len);
    TcpClient *tcpclient =new TcpClient(this);
    int nRet = tcpclient->ShortConnect(addr,port,sendData,len +2);
    if(nRet == len + 2)
    {
        nRet = len;
    }
    else
    {
        nRet = -200;
    }

    delete tcpclient;
    return nRet;
}

int NetWork::ClientUdp(QHostAddress addr,int port,const char *data,int len)
{
    char sendData[len +2];
    memset(sendData,0xFF,sizeof(sendData));
    memcpy(sendData+2,data,len);
    UdpClient udpclient(this);
    int nRet = udpclient.SendData(addr,port,sendData,len +2);
    if(nRet == len +2)
    {
        return len;
    }
}

int NetWork::TcpClientSendData(QHostAddress addr,const char *data,int len)
{
    CMapAddrObject::iterator iter = std::find_if(_mapAddrObject.begin(),_mapAddrObject.end(),find_Object(addr));
    if(iter == _mapAddrObject.end())
        return -100;

    char sendData[len +2];
    memset(sendData,0xFF,sizeof(sendData));
    memcpy(sendData+2,data,len);
    int nRet = iter->first->SendData(sendData,len +2);
    if(nRet == len +2)
    {
        return len;
    }
    return nRet;
}

int NetWork::TcpServerSendData(QHostAddress addr,const char *data,int len)
{
    CMapAddrObject::iterator iter = std::find_if(_mapAddrObject.begin(),_mapAddrObject.end(),find_Object(addr));
    if(iter == _mapAddrObject.end())
        return -100;

    char sendData[len +2];
    memset(sendData,0xFF,sizeof(sendData));
    memcpy(sendData+2,data,len);
    int nRet = iter->first->SendTcpData(addr,sendData,len +2);
    if(nRet == len+2)
    {
        return len;
    }
}

int NetWork::recvData(QHostAddress addr,int port,const char *data,int len)
{
    if(_mapAddrData.find(addr.toString()) == _mapAddrData.end())
    {
        if(data[0] != (char)0xFF || data[1] != (char)0xFF)
        {
            return -1000;
        }

        int leng = 0;
        memcpy(&leng,data + 2,sizeof(leng));
        if(leng == len - 2 -4)
        {
             _parent->dealWith(addr,data + 2,len -2);
        }
        else
        {
            QByteArray tmpData(data + 2,len -2);
            _mapAddrData.insert(std::make_pair(addr.toString(),tmpData));
        }
    }
    else
    {
        _mapAddrData[addr.toString()].append(data,len);
        int leng = 0;
        memcpy(&leng,_mapAddrData[addr.toString()].data(),sizeof(leng));
        if(leng == _mapAddrData[addr.toString()].length() - 4)
        {
            char tmp[_mapAddrData[addr.toString()].length()];
            memcpy(tmp,_mapAddrData[addr.toString()].data(),_mapAddrData[addr.toString()].length());
            _parent->dealWith(addr,tmp,_mapAddrData[addr.toString()].length());
            _mapAddrData.erase(_mapAddrData.find(addr.toString()));
        }
        else if(leng < _mapAddrData[addr.toString()].length() - 4)
        {
            _mapAddrData.erase(_mapAddrData.find(addr.toString()));
        }
    }
}

int NetWork::NewConnect(QHostAddress addr)
{
    _mapAddrObject.insert(std::make_pair(_server,addr));
}

int NetWork::CloseTcp(QHostAddress addr)
{
    CMapAddrObject::iterator iter = std::find_if(_mapAddrObject.begin(),_mapAddrObject.end(),find_Object(addr));
    if(iter == _mapAddrObject.end())
        return -100;
    iter->first->CloseConnect();
    _mapAddrObject.erase(iter);
}
