#ifndef LIST_H
#define LIST_H

#include "init.h"
#include <SDL2/SDL.h>

void init_list(struct window *window, enum list_type type);

void list_push_front(SDL_Rect *pos, struct window *window, enum list_type type);

void free_list(struct list *list);

#endif /* !LIST_H */
