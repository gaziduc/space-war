#ifndef GAME_H
#define GAME_H

#include "init.h"
#include <SDL2/SDL.h>

#define SHIP_SPEED 8
#define SHOT_SPEED 20
#define FRAMES_BETWEEN_ENEMIES 300

void play_game(struct window *window);

#endif /* !GAME_H */
