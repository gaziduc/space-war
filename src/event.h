#ifndef EVENT_H
#define EVENT_H

#include "init.h"
#include <SDL2/SDL.h>

void update_events(struct input *in);

void handle_quit_event(struct window *window);

#endif /* !INPUT_H */

