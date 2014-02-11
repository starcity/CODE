#ifndef NETOBJECT_H
#define NETOBJECT_H

class NetObject
{
public:
    NetObject(){}
    ~NetObject(){}

    virtual int SendData(const char *data,int len) = 0;
    virtual int SendTcpData(QHostAddress addr,const char *data,int len){};
    virtual void CloseConnect() = 0;
};

#endif // NETOBJECT_H
