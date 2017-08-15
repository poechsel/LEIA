#ifndef H_SCREEN
#define H_SCREEN


#include <SDL2/SDL.h>
#include "structs.h"

/* this function deals with the screen
 * machine -> the vm
 * force_quit -> to detect if we lust close the screen (or shut-down the simulator)
 * refr -> if true we must refresh the screen and wait. Otherwise nothing to be done
 */
void simulate_screen(const Machine& machine, bool &force_quit, volatile bool &refr, bool activate_refresh_command = true);

#endif
