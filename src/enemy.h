#ifndef ENEMY_H
#define ENEMY_H

#include "init.h"

#define FRAMES_BETWEEN_ENEMY_SHOTS 180
#define ENEMY_SHOT_SPEED 5

void set_enemy_attributes(struct list *new, SDL_Rect *pos, struct window *window);

void create_enemies(struct window *window);

void move_enemies(struct window *window);

void render_enemies(struct window *window);

void render_enemies_health(struct window *window);

void set_enemy_shot_pos(struct list *new, SDL_Rect *pos, struct window *window);

void move_enemy_shots(struct window *window);

void render_enemy_shots(struct window *window);

#endif /* !ENEMY_H */
