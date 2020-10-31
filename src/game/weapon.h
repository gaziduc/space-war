#ifndef WEAPON_H
#define WEAPON_H

#include "init.h"
#include <SDL2/SDL.h>

#define BOMB_DAMAGE 10

int shoot(struct window *window, struct player *player, int provoked_by_me);

void bomb(struct window *window, int provoked_by_me);

void choose_weapons(struct window *window, int selected_level, int selected_difficulty, const char *str);

int get_weapon_damage(int weapon);

int get_weapon_speed(int weapon);

int get_weapon_shots_per_second(int weapon);

#endif /* !WEAPON_H */
