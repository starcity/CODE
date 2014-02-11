#include "ChatUi.h"
#include "Chat_strcutinfo.h"
#include <QMessageBox>
#include <QMenu>
#include <QListWidget>
#include <QtDebug>
#include <QNetworkInterface>
#include <QHostInfo>
#include "Netinfo.h"



QMyChat::QMyChat()
{
     qDebug() << QThread::currentThreadId();

     dialog = new QWidget;
     setupUi(dialog);

     _useraddr.clear();
     _cmapdialog.clear();
     _net = new NetWork(this);

     listWidget->setContextMenuPolicy(Qt::CustomContextMenu);

     connect(listWidget,SIGNAL(customContextMenuRequested(const QPoint &)),this,SLOT(showMouseRight(const QPoint &)));   //鼠标右键信号槽
     connect(listWidget,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(onDoubleClick(QModelIndex)));//双击
     connect(dialog,SIGNAL(destroyed()),this,SLOT(quitMainDialog()));
}

QMyChat::~QMyChat()
{
    delete dialog;
}

void QMyChat::show()
{
    dialog->show();
}

void QMyChat::setDefalutSize(qint16 widget,qint16 height)
{
    dialog->resize(widget,height);
}

int QMyChat::start()
{
    _net->StartServer(TCP,QHostAddress::Any,8888); //TCP服务器创建

    _net->StartServer(UDP,QHostAddress::Broadcast,9999); //UDP服务器创建
    loginNewNotication(QHostAddress::Broadcast);
}

int QMyChat::sendData(char connectType,char type,QString addr,QByteArray data)
{    
    if(connectType == TCP)
    {
        QByteArray tmp ;
        tmp.append(type);
        tmp.append("1234",4);
//        quint16 len = data.length();
//        tmp.append((char *)&len,sizeof(quint16));
        tmp.append(data);
        int leng = tmp.length();
        tmp.insert(0,(char *)&leng,sizeof(leng));
        int nRet = _net->TcpShortConnect(QHostAddress(addr),8888,tmp.data(),tmp.length());
        if(nRet == tmp.length())
        {
              _net->CloseTcp(QHostAddress(addr));
              return data.length();
        }

        return -100;
    }
    else if(connectType == UDP)
    {
        QByteArray tmp ;
        tmp.append(type);
        tmp.append("1234",4);
//        quint16 len = data.length();
//        tmp.append((char *)&len,sizeof(quint16));
        tmp.append(data);
        int leng = tmp.length();
        tmp.insert(0,(char *)&leng,sizeof(leng));
        int nRet = _net->ClientUdp(QHostAddress::Broadcast,9999,tmp.data(),tmp.length());
        if(nRet == tmp.length())
        {
              _net->CloseTcp(QHostAddress(addr));
              return data.length();
        }

        return -100;
    }
}

int QMyChat::dealWith(QHostAddress addr,const char *data,int len)
{
    int length = 0;
    char type = 0;
    int id = 0;
    char datainfo[100] = {0};

    memcpy(&length,data,sizeof(length));
    type = data[MSGLENGTH];
    memcpy(&id,data + MSGLENGTH+1,sizeof(id));

    int header = MSGLENGTH + ID +TYPE;
    switch(type)
    {
       case OTHERUSERLOGIN:
            addFriends(addr,data + header ,len - header);
            break;
       case OTHERUSERLOGOUT:
            deleteFriends(addr,data + header,len - header);
            break;
       case SHOWFRIENDMSG:
            showFriendMsg(addr,data + header,len - header);
            break;
       case SENDFILEDATA:
            showFriendImageMsg(addr,data + header,len - header);
            break;
    }
}

bool QMyChat::addFriends(QHostAddress addr,const char *data,int len)
{
    QList<QHostAddress> list;
    list = QNetworkInterface::allAddresses();
     foreach(QHostAddress address,list)
     {
          if(address.protocol() == QAbstractSocket::IPv4Protocol)
              if(address == addr)
                  return 0;
     }
    CVUserAddr::iterator iter;
    iter =_useraddr.find(addr.toString());
    if(iter == _useraddr.end())
    {
       short int length =0;

       char str[50] = {0};
       memcpy(&length,data,2);
       memcpy(str,data+2,length);
       QString user(str);
       PeerInfo peerinfo;
       peerinfo._name = user.section('(',0,0);             //computer name
//       peerinfo._ip  = user.section('(',1,1).remove(')');  // ip
       peerinfo._ip = addr.toString();                  //ip

       short int maclen =0;
       memcpy(&maclen,data +2 +length,2);
       memset(str,0x00,sizeof(str));
       memcpy(str,data+2+length+2,maclen);
       peerinfo._mac = str;    //peer mac

       _useraddr.insert(std::make_pair(addr.toString(),peerinfo));

       listWidget->addItem(new QListWidgetItem(QIcon(""),user));
       loginNewNotication(addr);
    }
}

bool QMyChat::showFriendMsg(QHostAddress addr,const char *data,int len)
{
    CMapDialog::iterator iter = _cmapdialog.find(addr.toString());
    if(iter == _cmapdialog.end())
    {
        return 0;
    }
    else
    {
        iter->second->showFriendMsg(data,len);
    }
}

bool QMyChat::showFriendImageMsg(QHostAddress  addr,const char *data,int len)
{
    CMapDialog::iterator iter = _cmapdialog.find(addr.toString());
    if(iter == _cmapdialog.end())
    {
        return 0;
    }
    else
    {
        iter->second->showFriendImageMsg(data,len);
    }
}

bool QMyChat::deleteFriends(QHostAddress addr,const char *data,int len)
{
    short int length =0;

    memcpy(&length,data,2);
    char str[length +1];
    memset(str,0x00,sizeof(str));
    memcpy(str,data+2,length);
    QString user(str);

    QList<QListWidgetItem *> qlist = listWidget->findItems(user,Qt::MatchRecursive);
    if(qlist.empty() == false)
    {
        listWidget->takeItem(listWidget->row(qlist[0]));
        delete qlist[0];
    }

    CVUserAddr::iterator iter;
    iter = _useraddr.find(addr.toString());
    if(iter != _useraddr.end())
    {
       _useraddr.erase(iter);
    }

     return 0;
}

int QMyChat::loginNewNotication(QHostAddress addr)//发送登录通知
{
    QByteArray sendData;
    QString ip;
    ip = QHostInfo::localHostName();
    ip +="(";
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    foreach (QHostAddress address, list)
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
        {
            if(address.toString() != "127.0.0.1")
            {
                ip += address.toString();
                break;
            }
        }
    }
    ip +=")";
    short int tmplen = ip.length();
    sendData.append(ip);
    sendData.insert(0,(char *)&tmplen,sizeof(tmplen));

    QString mac;
    QList<QNetworkInterface> listInterface = QNetworkInterface::allInterfaces();
    mac = listInterface.at(0).hardwareAddress();                  //MAC地址；

    tmplen = mac.length();

    sendData.append((char *)&tmplen,sizeof(tmplen));
    sendData.append(mac);

    sendData.insert(0,"1234",4);                //ID
    char type = 1;
    sendData.insert(0,&type,1);            //协议类型  1 登录消息

    int leng = sendData.length();

    sendData.insert(0,(char *)&leng,sizeof(leng));                   // 数据长度

    int nRet = _net->ClientUdp(addr,9999,sendData.data(),sendData.length());
    if(nRet != sendData.length())
    {
        return -120;
    }
}

void QMyChat::showMouseRight(const QPoint &)
{
    QMenu *qMenu = NULL;

    if (qMenu)
    {
         delete qMenu;
         qMenu = NULL;
    }

     qMenu = new QMenu(listWidget);
     if(listWidget->itemAt(listWidget->mapFromGlobal(QCursor::pos())) == NULL)
     {
         QAction* flashPaneAction = new QAction("&刷新列表",this);
         QAction* openItemsAction = new QAction("&展开列表", this);
         QAction* addGroupAction = new QAction("&添加分组", this);

         qMenu->addAction(flashPaneAction);
         qMenu->addAction(openItemsAction);
         qMenu->addAction(addGroupAction);
     }
     else
     {
         QAction* chatPaneAction = new QAction("&聊天",this);
         QAction* showInfoAction = new QAction("&显示好友信息", this);

         qMenu->addAction(chatPaneAction);
         qMenu->addAction(showInfoAction);
     }

     qMenu->exec(QCursor::pos()); //在鼠标点击的位置显示鼠标右键菜单
}

void QMyChat::onDoubleClick(QModelIndex modeIndex)
{
     QString str = modeIndex.data().toString().section('(',1,1).remove(')');

     if(_cmapdialog.find(str) == _cmapdialog.end())
     {
         QchatDialog *_chatDialog = new QchatDialog(this);

         _cmapdialog.insert(std::make_pair(str,_chatDialog));
         CVUserAddr::iterator iter = _useraddr.find(str);
         if(iter != _useraddr.end())
         {
             _chatDialog->_dialogInfo.addr = iter->second._ip;
             _chatDialog->_dialogInfo.name = iter->second._name;
             _chatDialog->_dialogInfo.mac  = iter->second._mac;
         }

         _chatDialog->setWindowsTitle(modeIndex.data().toString());
         _chatDialog->setPeerInfo();
         _chatDialog->show();
         connect(_chatDialog,SIGNAL(sendCloseDialog(QchatDialog *)),this,SLOT(closeDialog(QchatDialog*)));
     }
}

void QMyChat::dealWithMsg(const QStringList str)
{
    QMessageBox::information(dialog,str[0],str[1]);
}

void QMyChat::closeDialog(QchatDialog *dialog)
{
    CMapDialog::iterator iter = std::find_if(_cmapdialog.begin(),_cmapdialog.end(),find_dialog(dialog));
    if(iter != _cmapdialog.end())
    {
        _cmapdialog.erase(iter);
    }
//    delete dialog;
    dialog = NULL;
}

void QMyChat::quitMainDialog()
{
    QByteArray sendData;
    QString ip;
    ip = QHostInfo::localHostName();
    ip +="(";
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    foreach (QHostAddress address, list)
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
        {
            if(address.toString() != "127.0.0.1")
            {
                ip += address.toString();
                break;
            }
        }
    }
    ip +=")";
    short int tmplen = ip.length();
    sendData.append(ip);
    sendData.insert(0,(char *)&tmplen,sizeof(tmplen));

    sendData.insert(0,"1234",4);                //ID
    char type = 2;
    sendData.insert(0,&type,1);            //协议类型  2 下线消息

    int leng = sendData.length();

    sendData.insert(0,(char *)&leng,sizeof(leng));                   // 数据长度

    int nRet = _net->ClientUdp(QHostAddress::Broadcast,9999,sendData.data(),sendData.length());
    if(nRet != sendData.length())
    {
        return ;
    }
}
