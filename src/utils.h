#ifndef UTILS_H
#define UTILS_H

#include "init.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define POS_CENTERED -1

SDL_Texture *load_texture(const char *path, struct window *window);

struct collision_texture *load_texture_collision(const char *path, struct window *window);

void init_position(int x, int y, struct window *window, SDL_Texture *texture,
                   SDL_Rect *pos);

void error(const char *title, const char *text, SDL_Window *window);

void *xmalloc(size_t size, SDL_Window *window);

void *xcalloc(size_t nmenb, size_t size, SDL_Window *window);

void *xrealloc(void *ptr, size_t size, SDL_Window *window);

SDL_Texture *get_text_texture(struct window *window, TTF_Font *font,
                              const char *text, SDL_Color fg);

TTF_Font *load_font(struct window *window, const char *filename,
                    int pt_size);

Mix_Chunk *load_sound(struct window *window, const char *filename);

#endif /* !UTILS_H */
