#ifndef SDLWIDGET_H
#define SDLWIDGET_H

#include <QObject>
#include <QWidget>
#include <SDL2/SDL.h>
#undef main

class SDLWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SDLWidget(QWidget *parent = nullptr);

signals:

public slots:
};

#endif // SDLWIDGET_H
