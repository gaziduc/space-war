#ifndef EFFECT_H
#define EFFECT_H

#define TOUCHED_EFFECT_MAX_ALPHA 96

#include "init.h"

void render_touched_effect(struct window *window);

void set_shake_effect(struct window *window);

void force_feedback(struct window *window, struct player *player, float strength, Uint32 length);

void force_feedback_on_all_controllers(struct window* window, float strength, Uint32 length);

#endif // EFFECT_H
