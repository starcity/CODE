#ifndef VIDEO_H
#define VIDEO_H
#include <QCamera>
#include <QCameraViewfinder>
#include <QEvent>
#include <QToolButton>


class Video:public QObject
{
    Q_OBJECT

public:
    Video();
    ~Video();

public:
    void Start();
    void Stop();

    void Resize(int width = 600,int heigth = 400);

    //inherit
    bool eventFilter(QObject * object, QEvent * event);

signals:
    void SendVideoStop();

private slots:
    void OnVideoDestory();
    void OnVideoError(QCamera::Error);

private:
    QCamera *_camera;
    QCameraViewfinder *_cameraViewFinder;
    QPoint _dragPosition;
    QToolButton *_pbClose;
};


#endif // VIDEO_H
