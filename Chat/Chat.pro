#-------------------------------------------------
#
# Project created by QtCreator 2013-09-05T21:26:30
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += multimedia
QT       += multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Chat
TEMPLATE = app


SOURCES += main.cpp \
    ChatUi.cpp \
    NetWork.cpp \
    chatDialog.cpp \
    TcpClient.cpp \
    Server.cpp \
    UdpClient.cpp \
    MyWidget.cpp \
    Video.cpp

HEADERS  += \
    ChatUi.h \
    NetWork.h \
    Chat_strcutinfo.h \
    chatDialog.h \
    ChatInfoStruct.h \
    TcpClient.h \
    Server.h \
    UdpClient.h \
    Netinfo.h \
    NetObject.h \
    MyWidget.h \
    Video.h

FORMS    += \
    Chat.ui \
    ChatDialog.ui

RC_FILE = myapp.rc

RESOURCES += \
    ico.qrc


