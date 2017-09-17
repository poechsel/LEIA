#ifndef SDLWIDGET_H
#define SDLWIDGET_H

#include <QObject>
#include <QWidget>
#include <SDL2/SDL.h>
#include "../simulateur/screen.h"
#include <QDebug>
#include <QTime>
#include <QThread>

#include <QPainter>




class QtScreen : public QWidget, public Screen
{
    Q_OBJECT
public:
    explicit QtScreen(QWidget *parent = nullptr);
    ~QtScreen();
    void updateContent(uword *memory);
    void paintEvent(QPaintEvent *event);

private:
    QRgb* _pixels;
    QImage _image;
    int _last_time;
    QTime _timer;
signals:

public slots:
};

#endif // SDLWIDGET_H
