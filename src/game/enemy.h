#ifndef ENEMY_H
#define ENEMY_H

#include "init.h"

#define FRAMES_BETWEEN_ENEMY_SHOTS 100
#define FRAMES_BETWEEN_ROTATING_ENEMY_SHOTS 8
#define FRAMES_BETWEEN_DRONE_SHOTS 60
#define ENEMY_SHOT_SPEED 8
#define FINAL_BOSS_SHOT_SPEED 14

void set_enemy_attributes(struct list *new, SDL_Rect *pos,
                          struct window *window, char enemy_type);

void create_enemies(struct window *window);

struct player* select_player(struct window *window, struct list *temp);

void move_enemies(struct window *window);

void render_enemies(struct window *window);

void render_enemy_health(struct window *window, struct list *enemy);

void render_enemies_health(struct window *window);

void set_enemy_shot_attributes(struct list *new, SDL_Rect *pos, struct window *window,
                               SDL_Rect *ship_pos, char enemy_type);

void move_enemy_shots(struct window *window);

void render_enemy_shots(struct window *window);

#endif /* !ENEMY_H */