#ifndef LIST_H
#define LIST_H

#include "init.h"
#include <SDL2/SDL.h>

void init_list(struct window *window, enum list_type type);

void list_push_front(SDL_FRect *pos, struct window *window,
                     enum list_type type, SDL_Texture *texture,
                     SDL_FRect *ship_pos, enum object_type object,
                     char enemy_type, int explosion_num);

void free_list(struct list *list);

void clear_list(struct list *list);

#endif /* !LIST_H */
