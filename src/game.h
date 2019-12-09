#ifndef GAME_H
#define GAME_H

#include "init.h"
#include <SDL2/SDL.h>

#define SHIP_SPEED 8
#define SHOT_SPEED 20
#define MAX_HEALTH 200

void render_trail(struct window *window, SDL_Rect *pos, int is_enemy);

void play_game(struct window *window);

#endif /* !GAME_H */
