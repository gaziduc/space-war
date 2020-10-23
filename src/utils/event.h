#ifndef EVENT_H
#define EVENT_H

#include "init.h"
#include <SDL2/SDL.h>

#define DEAD_ZONE 8196

void update_events(struct input *in, struct window *window);

void handle_quit_event(struct window *window, int is_in_level);

int handle_escape_event(struct window *window);

int handle_play_event(struct window *window);

void handle_select_arrow_event(struct window *window, unsigned *selected, unsigned max, SDL_Rect areas[]);

void init_controller(struct input *in, Sint32 which);

int handle_focus_lost_event(struct window *window);

#endif /* !INPUT_H */

