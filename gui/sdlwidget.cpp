#include "sdlwidget.h"

SDLWidget::SDLWidget(QWidget *parent) : QWidget(parent)
{
     char windowid[64];
#ifdef Q_WS_WIN

            sprintf(windowid, "SDL_WINDOWID=0x%lx", reinterpret_cast<qlonglong>(winId()));
#elif defined Q_WS_X11
            sprintf(windowid, "SDL_WINDOWID=0x%lx", winId());
#else
            qFatal("Fatal: cast du winId() inconnu pour votre plate-forme; toute information est la bienvenue!");
#endif
            SDL_putenv(windowid);

}
