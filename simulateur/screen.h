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
    virtual void update(uword *memory) = 0;
};

class SDLScreen : public Screen {
    public:
        SDLScreen();
        ~SDLScreen();
        virtual void update(uword *memory);
    private:
        void action(SDLScreen *screen);
        volatile bool _refresh;
        bool _force_quit;
        uword *_memory;
        std::thread _thread;

};

/* this function deals with the screen
 * machine -> the vm
 * force_quit -> to detect if we lust close the screen (or shut-down the simulator)
 * refr -> if true we must refresh the screen and wait. Otherwise nothing to be done
 */
void simulate_screen(const Machine& machine, bool &force_quit, volatile bool &refr, bool activate_refresh_command = true);

#endif
