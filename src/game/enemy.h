#ifndef ENEMY_H
#define ENEMY_H

#include "init.h"

#define FRAMES_BETWEEN_ENEMY_SHOTS 100
#define FRAMES_BETWEEN_ROTATING_ENEMY_SHOTS 10
#define FRAMES_BETWEEN_DRONE_SHOTS 60
#define ENEMY_SHOT_SPEED 8
#define FINAL_BOSS_SHOT_SPEED 15

void set_enemy_attributes(struct list* new, SDL_FRect* pos,
                          struct window* window, char enemy_type,
                          float speed_x, int health, int max_health,
                          int override_pos_and_speed, float speed_y);

void create_enemies(struct window *window);

struct player* select_player(struct window *window, struct list *temp);

void move_enemies(struct window *window);

void render_enemies(struct window *window);

void render_enemy_health(struct window *window, struct list *enemy, Uint8 alpha);

void render_enemies_health(struct window *window);

void set_enemy_shot_attributes(struct list *new, SDL_FRect *pos,
                               SDL_FRect *ship_pos, char enemy_type, struct window *window,
                               SDL_FRect *override_speed, int override_pos);

void move_enemy_shots(struct window *window);

void render_enemy_shots(struct window *window);

#endif /* !ENEMY_H */
