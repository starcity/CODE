#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QColor>
#include "ui_ChatDialog.h"
#include "ChatUi.h"
#include "Video.h"


class QMyChat;

struct DialogInfo
{
    QString name;
    QString addr;
    QString mac;
    QString ostype;
};

class QchatDialog :public QObject,public Ui::ChatDialog
{
    Q_OBJECT

public:
    QchatDialog(QMyChat *parent = NULL);
    ~QchatDialog();

signals:
    void sendCloseDialog(QchatDialog *dialog);//close dialog


public slots:
    void show();
    void closeDialog();//send signal
    void sendFriendMsg();
    void onTextChanged();
    void OnBold();
    void OnItalic();
    void OnUnderlined();
    void OnColor();
    void OnTextModifyChanged();

    //toolbutton
    void OnVideoChat();
    void OnCloseVideo();

public:
    void setWindowsTitle(const QString &title);
    void setPeerInfo();
    void showFriendMsg(const char *data,int len);
    void showFriendImageMsg(const char *data,int len);

private:
    int sendData(char connectType,char type,QByteArray data);

public:
    DialogInfo _dialogInfo;

private:
    QWidget *_dialog;
    QMyChat *_parent;
    QString *_msg;
    QColor  _color;
    bool   _isBold;
    bool   _isItalic;
    bool   _isUnderLined;
    Video  *_video;
};

#endif // CHATDIALOG_H
