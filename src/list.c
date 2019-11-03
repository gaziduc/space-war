#include "list.h"
#include "utils.h"

void init_list(struct window *window)
{
    // Creating list
    window->list = xmalloc(sizeof(struct list), window->window);

    // Sentinel = list, So we don't care about list->pos
    // list->next is set to NULL because there is nothing in the list
    window->list->next = NULL;
}

void list_push_front(SDL_Rect *pos, struct window *window)
{
    // Allocating memory for new shot
    struct list *new = xmalloc(sizeof(struct list), window->window);

    // Setting shot initial position
    int w = 0;
    int h = 0;
    SDL_QueryTexture(window->img->ship, NULL, NULL, &w, &h);
    new->pos.x = pos->x + w;
    new->pos.y = pos->y + h / 2;

    SDL_QueryTexture(window->img->shot, NULL, NULL, &new->pos.w, &new->pos.h);
    new->pos.x -= new->pos.w;
    new->pos.y -= new->pos.h / 2;

    // Pushing element in front of the list
    new->next = window->list->next;
    window->list->next = new;
}

void free_list(struct list *list)
{
    if (list->next)
        free_list(list->next);

    free(list);
}
