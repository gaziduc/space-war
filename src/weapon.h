#ifndef WEAPON_H
#define WEAPON_H

#include "init.h"
#include <SDL2/SDL.h>

#define BOMB_DAMAGE 10

int shoot(struct window *window, struct player *player);

void bomb(struct window *window);

#endif /* !WEAPON_H */
