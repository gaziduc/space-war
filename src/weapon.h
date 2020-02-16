#ifndef WEAPON_H
#define WEAPON_H

#include "init.h"
#include <SDL2/SDL.h>

#define BOMB_DAMAGE 10

void shoot(struct window *window, SDL_Rect *pos);

void bomb(struct window *window);

#endif /* !WEAPON_H */
