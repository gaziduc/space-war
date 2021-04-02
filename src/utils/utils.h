#ifndef UTILS_H
#define UTILS_H

#include "init.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_framerate.h>

#define POS_CENTERED -4200

SDL_Texture *load_texture(const char *path, struct window *window);

void frame_delay(FPSmanager *fps);

struct collision_texture *get_texture_collision(SDL_Surface *surface, struct window *window);

struct collision_texture *load_texture_collision(const char *path, struct window *window);

void init_position(int x, int y, SDL_Texture *texture, SDL_Rect *pos);

void init_position_float(int x, int y, SDL_Texture *texture, SDL_FRect *pos);

void error(const char *title, const char *text, SDL_Window *window, SDL_Renderer *renderer);

void *xmalloc(size_t size, SDL_Window *window, SDL_Renderer *renderer);

void *xcalloc(size_t nmenb, size_t size, SDL_Window *window, SDL_Renderer *renderer);

void *xrealloc(void *ptr, size_t size, SDL_Window *window, SDL_Renderer *renderer);

SDL_Texture *get_text_texture(struct window *window, TTF_Font *font,
                              const char *text, SDL_Color fg);

void render_text(struct window *window, TTF_Font *font, const char *text, SDL_Color fg,
                 int x, int y);

TTF_Font *load_font(struct window *window, const char *filename,
                    int pt_size);

Mix_Chunk *load_sound(struct window *window, const char *filename);

void resize_pos_for_resolution(struct window *window, SDL_Rect *pos);

void resize_pos_for_resolution_float(struct window *window, SDL_FRect *pos);

#endif /* !UTILS_H */
