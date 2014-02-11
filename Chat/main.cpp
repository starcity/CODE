#include "ChatUi.h"
#include <QApplication>
#include <QNetworkInterface>
#include <QFile>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    app.setWindowIcon(QIcon("lightning.ico"));
//    app.setStyleSheet("*{background-color:red;}");

    QchatDialog w;
    w.show();

//    QMyChat chat;
//    chat.setDefalutSize(217,500);
//    chat.show();

//    chat.start();
   // chat.addFriends("历史人物");

    return app.exec();
}
