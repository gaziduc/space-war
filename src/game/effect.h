#ifndef EFFECT_H
#define EFFECT_H

#define TOUCHED_EFFECT_MAX_ALPHA 96

#include "init.h"

void render_touched_effect(struct window *window);

void set_shake_effect(struct window *window);

void force_feedback(struct window *window, struct player *player, float strength, Uint32 length);

void force_feedback_on_all_controllers(struct window* window, float strength, Uint32 length);

/*
* Particle effects (purely visual, local to each player, never sent on the network)
*/

/* Sparks + smoke flying out of an explosion centered on (x, y). scale: 1.0 = big explosion */
void spawn_explosion_particles(float x, float y, float scale);

/* Small flame behind a ship, called every frame for each living player */
void spawn_engine_particles(const SDL_FRect *ship_pos);

/* Expanding shockwave ring + white flash, used by the bomb */
void spawn_shockwave(float x, float y);

/* Moves and displays all particles and shockwaves */
void update_and_render_particles(struct window *window);

/* Removes all particles (new level, retry...) */
void clear_particles(void);

#endif // EFFECT_H
