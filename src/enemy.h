#ifndef ENEMY_H
#define ENEMY_H

#include "init.h"

#define ENEMY_SPEED 3

void set_enemy_pos(struct list *new, struct window *window);

void move_enemies(struct window *window);

void render_enemies(struct window *window);

#endif /* !ENEMY_H */
