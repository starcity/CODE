#include "Video.h"
#include <QMessageBox>
#include <QMouseEvent>
#include <iostream>


Video::Video()
{
    _camera = new QCamera();
    _cameraViewFinder = new QCameraViewfinder();


    _camera->setViewfinder(_cameraViewFinder);
    _cameraViewFinder->setAttribute(Qt::WA_DeleteOnClose);
    _cameraViewFinder->setWindowFlags(Qt::FramelessWindowHint);
    _cameraViewFinder->installEventFilter(this);

    connect(_cameraViewFinder,SIGNAL(destroyed()),this,SLOT(OnVideoDestory()));
    connect(_camera,SIGNAL(error(QCamera::Error)),this,SLOT(OnVideoError(QCamera::Error)));

    //初始化
    _pbClose = NULL;

    Resize(200,150);
}

Video::~Video()
{
    if(_camera != NULL)
    {
        Stop();
        delete _camera;
    }
    if(_cameraViewFinder != NULL)
    {
        delete _cameraViewFinder;
    }
}

void Video::Start()
{
    _cameraViewFinder->show();
    if(!QCamera::availableDevices().size())
    {
        emit _camera->error(QCamera::ServiceMissingError);
        return;
    }
    _camera->start();
}

void Video::Stop()
{
    _camera->stop();
}

void Video::Resize(int width,int heigth)
{
    _cameraViewFinder->resize(width,heigth);
}

void Video::OnVideoDestory()
{
    _cameraViewFinder = NULL;

    emit SendVideoStop();
}

void Video::OnVideoError(QCamera::Error error)
{
    switch(error)
    {
        case QCamera::InvalidRequestError:
        case QCamera::NotSupportedFeatureError:
             QMessageBox::warning(_cameraViewFinder,tr("警告"),tr("该功能不支持"));
            break;
        case QCamera::ServiceMissingError:
             QMessageBox::warning(_cameraViewFinder,tr("警告"),tr("系統沒有提供攝像設備"));
            break;
    }
}

bool Video::eventFilter(QObject * object, QEvent * event)
{
    if(_cameraViewFinder == object )
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *tmp = (QMouseEvent *)event;
            if(tmp->buttons() & Qt::LeftButton)
            {
               _dragPosition = tmp->globalPos() - _cameraViewFinder->frameGeometry().topLeft();
            }
            return true;
        }
        if(event->type() == QEvent::MouseMove)
        {
            if(!_cameraViewFinder->isFullScreen())
            {
                QMouseEvent *tmp = (QMouseEvent *)event;
                if(tmp->buttons() & Qt::LeftButton)
                {
                    _cameraViewFinder->move(tmp->globalPos() - _dragPosition);
                }
             }
            return true;
        }
        if(event->type() == QEvent::Enter)
        {
            if(_pbClose == NULL)
            {
                _pbClose = new QToolButton(_cameraViewFinder);
                _pbClose->setGeometry(_cameraViewFinder->width() - 20,0,20,20);
                _pbClose->setIcon(QPixmap(":/:/VideoClose"));
                _pbClose->setToolTip("关闭");
                _pbClose->setAttribute(Qt::WA_TranslucentBackground);
                connect(_pbClose,SIGNAL(clicked()),_cameraViewFinder,SLOT(close()));
                _pbClose->show();
            }
            return true;
        }
        if(event->type() == QEvent::Leave)
        {
            if(_pbClose != NULL)
            {
                _pbClose->close();
                delete _pbClose;
                _pbClose = NULL;
            }

            return true;
        }
        if(event->type() == QEvent::MouseButtonDblClick)
        {
            if(_cameraViewFinder->isFullScreen())
            {
                _cameraViewFinder->showNormal();
            }
            else
            {
                _cameraViewFinder->showFullScreen();
            }
            _pbClose->setGeometry(_cameraViewFinder->width() - 20,0,20,20);
            return true;
        }
    }

     return _cameraViewFinder->eventFilter(object,event);
}

