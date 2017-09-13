#ifndef H_SCREEN
#define H_SCREEN


#include <SDL2/SDL.h>
#include "structs.h"
#include <thread>
#include <iostream>
#include <cstdio>
class Screen {
    public:
        virtual ~Screen() {};
    virtual void updateContent(uword *memory) = 0;
};

class SDLScreen : public Screen {
    public:
        SDLScreen();
        ~SDLScreen();
        virtual void updateContent(uword *memory);
    protected:
        void action();
        virtual void createsdl();
        virtual void closesdl();
        SDL_Window *_window;
        SDL_Renderer *_renderer;
        SDL_Texture *_texture;

    private:

        volatile bool _refresh;
        bool _force_quit;
        uword *_memory;
        std::thread _thread;

};


#endif
