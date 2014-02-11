#include "chatDialog.h"
#include "Netinfo.h"
#include "Chat_strcutinfo.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QColorDialog>
#include <QFont>
#include <QMessageBox>




QchatDialog::QchatDialog(QMyChat *parent)
{
    _parent = parent;
    _dialog = new QWidget();
    _dialog->setAttribute(Qt::WA_DeleteOnClose);

    //初始化
    _isBold = false;
    _isItalic = false;
    _isUnderLined = false;

    _video = NULL;


    setupUi(_dialog);

    //添加提示信息
    tbB->setToolTip(tr("加粗"));
    tbI->setToolTip(tr("斜体"));
    tbU->setToolTip(tr("下划线"));
    tbC->setToolTip(tr("字體顏色"));
    tbVoice->setToolTip(tr("語音聊天"));
    tbVideo->setToolTip(tr("視頻聊天"));
    tbNetHelp->setToolTip(tr("遠程協助"));
    tbShare->setToolTip(tr("共享文件"));
    tbMedia->setToolTip(tr("多媒體娛樂"));

    //sendMsg快捷键设置
    send->setShortcut(tr("ctrl+return"));

    //关闭对话框信号---槽
    connect(close,SIGNAL(clicked()),_dialog,SLOT(close()));
    connect(_dialog,SIGNAL(destroyed()),this,SLOT(closeDialog()));

    //发送信息信息信号---槽
    connect(send,SIGNAL(clicked()),this,SLOT(sendFriendMsg()));
    connect(sendMsg,SIGNAL(textChanged()),this,SLOT(onTextChanged()));

    //修饰字体
    connect(tbB,SIGNAL(clicked()),this,SLOT(OnBold()));
    connect(tbI,SIGNAL(clicked()),this,SLOT(OnItalic()));
    connect(tbU,SIGNAL(clicked()),this,SLOT(OnUnderlined()));
    connect(tbC,SIGNAL(clicked()),this,SLOT(OnColor()));
    connect(fontBox,SIGNAL(currentFontChanged(QFont)),this,SLOT(OnTextModifyChanged()));
    connect(comBoxSize,SIGNAL(currentIndexChanged(int)),this,SLOT(OnTextModifyChanged()));

    //toolbutton click
    connect(tbVideo,SIGNAL(clicked()),this,SLOT(OnVideoChat()));
}

QchatDialog::~QchatDialog()
{
    if(_dialog != NULL)
    {
        delete _dialog;
    }
    if(_video != NULL)
    {
        delete _video;
    }
}

void QchatDialog::show()
{
    _dialog->show();
}

void QchatDialog::setWindowsTitle(const QString &title)
{
    _dialog->setWindowTitle(title);
}

void QchatDialog::closeDialog()
{
    qDebug() << QThread::currentThreadId();

    _dialog = NULL;
    emit sendCloseDialog(this);
}

void QchatDialog::sendFriendMsg()
{
    QString html = sendMsg->toPlainText();

    if(html.isEmpty())
        return ;
    html = sendMsg->toHtml();

    html.replace(QString("\\"), QString("/"));
    html.remove("file:///");

    int cout = html.count("src=");

    int indexStart = 0;
    int indexEnd = 0;

    for(int i=0;i < cout ;i++)
    {
        indexStart = html.indexOf("src=",indexStart);
        indexEnd = html.indexOf("\"",indexStart + 5);
        QFileInfo fileInfo(html.mid(indexStart + 5,indexEnd - indexStart -5));

        QString filename(TMPFILEDIRNAME);
        filename += fileInfo.fileName();

        html.remove(indexStart + 5,indexEnd - indexStart -5);
        html.insert(indexStart + 5,filename);

        QDir dir;
        bool exist = dir.exists(filename.mid(0,6));
        if(exist == false)
        {
            dir.mkdir(filename.mid(0,6));
        }
        QFile::copy(fileInfo.absoluteFilePath(),filename);

        QFile file(filename);
        if(!file.open(QIODevice::ReadOnly/*| QIODevice::Text*/))
        {
            break;
        }
        QByteArray filedata;
        filedata.append(0x01);
        short int  len = fileInfo.fileName().length();
        filedata.append((char *)&len,sizeof(len));
        filedata.append(fileInfo.fileName());
        filedata.append(file.readAll());

        file.close();

        int nRet = sendData(0,SENDFILEDATA,filedata);
        if(nRet != filedata.length())
        {
            showMsg->setTextColor(Qt::blue);
            showMsg->insertPlainText("Send data error\n");
            showMsg->setTextColor(Qt::black);
            return ;
        }
        indexStart ++;
    }

    html +='\n';
    QByteArray sendTmp;
    sendTmp.append(html);
    int nRet = sendData(0,SENDFRIENDMSG,sendTmp);
    if(nRet != sendTmp.length())
    {
        QString text;
        if(sendTmp.length() > 100)
        {
            text ="\nSend Message Failed:";
        }
        else
        {
            text ="\nSend Message Failed:" +html;
        }

        showMsg->setTextColor(Qt::blue);
        showMsg->insertPlainText(text);
        showMsg->setTextColor(Qt::black);
    }
    else
    {
        QString text;
        text ="\n\nMe Say:\n";
        QFont fontBefore = showMsg->currentFont();
        showMsg->setTextColor(Qt::red);
        showMsg->insertPlainText(text);
        text ="\t" + html;

        QFont  font;
        font = sendMsg->currentFont();
        showMsg->setCurrentFont(font);
        showMsg->insertHtml(text);

        fontBefore.setPointSize(12);
        showMsg->setCurrentFont(fontBefore);
    }

    QTextCursor textcursor = showMsg->textCursor();
    textcursor.setPosition(showMsg->toPlainText().length());
    showMsg->setTextCursor(textcursor);

    sendMsg->setPlainText("");
    sendMsg->setFocus();
}

int QchatDialog::sendData(char connectType,char type,QByteArray data)
{
    QString addr = _dialog->windowTitle();
    addr = addr.section('(',1,1);
    addr.remove(')');
    return  _parent->sendData(connectType,type,addr,data);
}

void QchatDialog::setPeerInfo()
{
    QLabel *label = new QLabel(friendInfo);
    QByteArray str;
    str.append("\n主机名:");
    str.append(_dialogInfo.name);
    str.append("\nIP地址:");
    str.append(_dialogInfo.addr);
    str.append("\nMAC地址:");
    str.append(_dialogInfo.mac);

    label->setText(str);
}

void QchatDialog::showFriendMsg(const char *data,int len)
{
    QString text;
    showMsg->setTextColor(Qt::green);
    text +="\n";
    text += _dialogInfo.name + " Say:\n   ";
    showMsg->insertPlainText(text);
    showMsg->setTextColor(Qt::black);

    text ="   " + QString::fromUtf8(data,len) + "\n";

//    showMsg->insertPlainText(text);

    showMsg->insertHtml(text);

    QTextCursor textcursor = showMsg->textCursor();
    textcursor.setPosition(showMsg->toPlainText().length());
    showMsg->setTextCursor(textcursor);
}

void QchatDialog::showFriendImageMsg(const char *data,int len)
{
    QDir dir;
    bool exist = dir.exists(TMPFILEDIRNAME);
    if(exist == false)
    {
        dir.mkdir(TMPFILEDIRNAME);
    }

    QByteArray fileData(data,len);
    char type = fileData[0];
    unsigned short fileLen =0;
    memcpy(&fileLen,fileData.data() + 1,2);

    char tmpName[fileLen +1];
    memset(tmpName,0x00,sizeof(tmpName));

    memcpy(tmpName,fileData.data() + 3,fileLen);

    QString fileName(TMPFILEDIRNAME);
    fileName.append(tmpName);

    fileData.remove(0,3 + fileLen);
    if(type == TMPFILETYPE)
    {
        QFile file(fileName);
        file.open(QIODevice::WriteOnly | QIODevice::Truncate);//新建文件，若有文件则覆盖
        int nRet = file.write(fileData.data(),fileData.length());
        if(nRet != fileData.length())
        {
            file.close();
            return ;
        }

        file.close();
    }
    else if(type == STOREFILETYPE)
    {
        //todu
    }
}

void QchatDialog::onTextChanged()
{
    QString msg;
    msg = sendMsg->toPlainText();
    int count = msg.count();
    if(count > INPUTMAXNUMBER)
    {
        msg.remove(INPUTMAXNUMBER,count - INPUTMAXNUMBER);
        sendMsg->setText(msg);

        QTextCursor textCursor = sendMsg->textCursor();
        textCursor.setPosition(INPUTMAXNUMBER);
        sendMsg->setTextCursor(textCursor);
    }
    QString html = sendMsg->toHtml();

    if(html.indexOf("file:///",0) != -1)
    {
        html.replace(QString("\\"), QString("/"));
        html.remove("file:///");
        sendMsg->setHtml(html);
    }
}

void QchatDialog::OnBold()
{
    tbB->setDown(!_isBold);
    _isBold = tbB->isDown();

    QFont font = sendMsg->currentFont();
    font.setBold(_isBold);
    sendMsg->selectAll();
    sendMsg->setCurrentFont(font);
    QTextCursor textCursor = sendMsg->textCursor();
    textCursor.setPosition(sendMsg->toPlainText().length());
    sendMsg->setTextCursor(textCursor);
}

void QchatDialog::OnItalic()
{
    tbI->setDown(!_isItalic);
    _isItalic = tbI->isDown();

   QFont font = sendMsg->currentFont();
   font.setItalic(_isItalic);
   sendMsg->selectAll();
   sendMsg->setCurrentFont(font);

   QTextCursor textCursor = sendMsg->textCursor();
   textCursor.setPosition(sendMsg->toPlainText().length());
   sendMsg->setTextCursor(textCursor);
}

void QchatDialog::OnUnderlined()
{
    tbU->setDown(!_isUnderLined);
    _isUnderLined = tbU->isDown();

    QFont font = sendMsg->currentFont();
    font.setUnderline(_isUnderLined);
    sendMsg->selectAll();
    sendMsg->setCurrentFont(font);

    QTextCursor textCursor = sendMsg->textCursor();
    textCursor.setPosition(sendMsg->toPlainText().length());
    sendMsg->setTextCursor(textCursor);
}

void QchatDialog::OnColor()
{
    _color = QColorDialog::getColor();

    sendMsg->selectAll();
    sendMsg->setTextColor(_color);
    QTextCursor textCursor = sendMsg->textCursor();
    textCursor.setPosition(sendMsg->toPlainText().length());
    sendMsg->setTextCursor(textCursor);
}

void QchatDialog::OnTextModifyChanged()
{
    QFont font;
    font = fontBox->currentFont();
    font.setPointSize(comBoxSize->itemText(comBoxSize->currentIndex()).toInt());
    sendMsg->selectAll();
    sendMsg->setCurrentFont(font);

    QTextCursor textCursor = sendMsg->textCursor();
    textCursor.setPosition(sendMsg->toPlainText().length());
    sendMsg->setTextCursor(textCursor);
}

void QchatDialog::OnVideoChat()
{
    if(_video == NULL)
    {
        _video = new Video;
        _video->Start();
        connect(_video,SIGNAL(SendVideoStop()),this,SLOT(OnCloseVideo()));
    }
}

void QchatDialog::OnCloseVideo()
{
    delete _video;
    _video = NULL;
}



