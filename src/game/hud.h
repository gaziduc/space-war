#ifndef HUD_H
#define HUD_H

#include "init.h"

#define SCORE_TIME 1000

void render_hud(struct window *window);

void set_hud_text(struct list* new, SDL_FRect* pos_dst, struct window* window, int override_pos_and_time, Uint32 last_time_hurt);

void move_hud_texts(struct window *window);

void render_hud_texts(struct window *window);

#endif /* !HUD_H */
