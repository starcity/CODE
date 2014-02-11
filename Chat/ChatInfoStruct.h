#ifndef CHATINFOSTRUCT_H
#define CHATINFOSTRUCT_H
#include <map>
#include <QString>
#include <iostream>

using namespace std;

typedef struct peerInfo
{
    QString _ip;
    QString _name;
    QString _mac;
}PeerInfo;


#define TMPFILEDIRNAME          "./tmp/"

#endif // CHATINFOSTRUCT_H
