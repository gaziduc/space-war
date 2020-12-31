#ifndef SHOT_H
#define SHOT_H

#include "init.h"
#include <SDL2/SDL.h>

void set_shot_pos(struct list *new, SDL_FRect *pos, struct window *window);

unsigned compute_combo_score(unsigned combo);

void end_combo(struct window *window);

void move_shots(struct window *window);

void render_shots(struct window *window);

#endif /* !SHOT_H */
