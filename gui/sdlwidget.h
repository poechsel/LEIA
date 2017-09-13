#ifndef SDLWIDGET_H
#define SDLWIDGET_H

#include <QObject>
#include <QWidget>

class SDLWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SDLWidget(QWidget *parent = nullptr);

signals:

public slots:
};

#endif // SDLWIDGET_H