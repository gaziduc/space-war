#ifndef WEAPON_H
#define WEAPON_H

#include "init.h"
#include <SDL2/SDL.h>

void shoot(struct window *window, SDL_Rect *pos);

void bomb(struct window *window);

#endif /* !WEAPON_H */
