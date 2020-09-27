#ifndef CONTROLS_H
#define CONTROLS_H

#include "init.h"
#include <SDL2/SDL.h>

void controls(struct window *window);

SDL_Scancode get_key(struct window *window, int selected_item, char *names[NUM_CONTROLS]);

#endif // CONTROLS_H
