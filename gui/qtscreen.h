#ifndef SDLWIDGET_H
#define SDLWIDGET_H

#include <QObject>
#include <QWidget>
#include <SDL2/SDL.h>
#include "../simulateur/screen.h"
#include <QDebug>

#include <QPainter>

#undef main
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
signals:

public slots:
};

#endif // SDLWIDGET_H
