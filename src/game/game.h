#ifndef GAME_H
#define GAME_H

#include "init.h"
#include <SDL2/SDL.h>

#define SHIP_SPEED 10
#define MAX_HEALTH_EASY 200
#define MAX_HEALTH_HARD 70
#define MAX_HEALTH_REALLY_HARD  30

void render_trail(struct window *window, struct player *player, SDL_FRect *pos,
                  int is_lan_player, int is_enemy);

void reset_game_attributes(struct window *window, int all_reset);

void play_game(struct window *window);

void quit_game(struct window* window);

#endif /* !GAME_H */
