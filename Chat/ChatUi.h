#ifndef CHATUI_H
#define CHATUI_H
#include "ui_Chat.h"
#include <QtGui>
#include "NetWork.h"
#include <vector>
#include "chatDialog.h"
#include "ChatInfoStruct.h"
//#include "MyWidget.h"


class NetWork;
class QchatDialog;

typedef std::map<QString,PeerInfo> CVUserAddr;
typedef std::map<QString,QchatDialog *> CMapDialog;

class find_dialog
{
public:
    find_dialog(QchatDialog * dialog):_dialog(dialog){}
    bool operator ()(CMapDialog::value_type &pair)
    {
        return pair.second == _dialog;
    }

private:
    QchatDialog *_dialog;
};

class QMyChat :public QObject, public Ui::Chat
{
    Q_OBJECT

public:
    QMyChat();
    ~QMyChat();

public:
    void show();
    void setDefalutSize(qint16 widget,qint16 height);

public:
    int start();
    int dealWith(QHostAddress addr,const char *data,int len);
    int sendData(char connectType,char type,QString addr,QByteArray data);


private:
    bool addFriends(QHostAddress addr,const char *data,int len);
    bool deleteFriends(QHostAddress addr,const char *data,int len);
    int  loginNewNotication(QHostAddress addr);
    bool showFriendMsg(QHostAddress addr,const char *data,int len);
    bool showFriendImageMsg(QHostAddress  addr,const char *data,int len);

private slots:
    void showMouseRight(const QPoint &);
    void onDoubleClick(QModelIndex modeIndex);
    void dealWithMsg(const QStringList str);
    void closeDialog(QchatDialog *dialog);
    void quitMainDialog();

public:
    QWidget *dialog;

private:
//    CMapIdAddr _cmapidaddr;
//    QStringList users;
//    CVector_Thread _vector_thread;
//    QThread _netThread;

    NetWork *_net;
    CVUserAddr   _useraddr;
    CMapDialog _cmapdialog;
};

#endif // CHATUI_H
