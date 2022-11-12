#ifndef WAVE_H
#define WAVE_H

#include "init.h"

#define TIME_SHOW_TITLE 2000

void render_title(struct window* window, Uint8 alpha, char* override_title);

void render_wave_title(struct window *window);

#endif /* !WAVE_H */
