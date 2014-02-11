#ifndef MYWIDGET_H
#define MYWIDGET_H
#include <QWidget>

class MyWidget:public QWidget
{
    Q_OBJECT

public:
    MyWidget();
    ~MyWidget();

public:
    void closeEvent(QCloseEvent *);
};

#endif // MYWIDGET_H
