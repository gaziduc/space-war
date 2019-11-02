#ifndef UTILS_H
#define UTILS_H

#include "init.h"
#include <SDL2/SDL.h>

#define POS_CENTERED -1

SDL_Texture *load_texture(const char *path, struct window *window);

void init_position(int x, int y, struct window *window, SDL_Texture *texture,
                   SDL_Rect *pos);

void error(const char *title, const char *text, SDL_Window *window);

void *xmalloc(size_t size, SDL_Window *window);

void *xcalloc(size_t nmenb, size_t size, SDL_Window *window);

#endif /* !UTILS_H */
