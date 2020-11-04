#include "init.h"
#include "background.h"
#include "utils.h"
#include <stdlib.h>
#include <SDL2/SDL.h>

void init_background(struct window *window)
{
    window->stars = xmalloc(sizeof(struct point), window->window, window->renderer);
    window->stars->next = NULL;

    // Create some initial points
    for (int c = 0; c < 4 * DEFAULT_H; c++)
    {
        struct point *new = xmalloc(sizeof(struct point), window->window, window->renderer);

        new->x = rand() % DEFAULT_W;
        new->y = rand() % DEFAULT_H;
        new->z = (rand() % 5) + 1;
        new->opacity = rand() % 256;
        new->size = (rand() % 3) + 1;

        if (new->z > 4)
            new->opacity /= 2;

        new->next = window->stars->next;
        window->stars->next = new;
    }
}

void move_background(struct window *window, unsigned long framecount)
{
    struct point *p = window->stars->next;

    while (p)
    {
        if (framecount % p->z == 0)
            p->x--;

        // Delete point
        if (p->x + p->size - 1 < 0)
        {
            // Remove and create new without free and malloc
            p->x = DEFAULT_W - 1;
            p->y = rand() % DEFAULT_H;
            p->z = (rand() % 5) + 1;
            p->opacity = rand() % 256;
            p->size = (rand() % 3) + 1;

            if (p->z > 4)
                p->opacity /= 2;
        }

        p = p->next;
    }
}


void set_next_shake(int *curr_shake)
{
    *curr_shake = -(*curr_shake);

    if (*curr_shake > 0)
        (*curr_shake)--;
    else if (*curr_shake < 0)
        (*curr_shake)++;
}

void render_background(struct window *window)
{
    struct point *p = window->stars->next;

    while (p)
    {
        SDL_SetRenderDrawColor(window->renderer, p->opacity,
                               p->opacity, p->opacity, p->opacity);

        SDL_Rect pos = { .x = p->x + window->shake.x,
                         .y = p->y + window->shake.y,
                         .w = 3,
                         .h = 3
                       };

        resize_pos_for_resolution(window, &pos);

        if (p->size > 1)
        {
            pos.w = p->size;
            pos.h = p->size;
            SDL_RenderFillRect(window->renderer, &pos);
        }
        else
            SDL_RenderDrawPoint(window->renderer, pos.x, pos.y);

        p = p->next;
    }

    set_next_shake(&window->shake.x);
    set_next_shake(&window->shake.y);
}


void free_background(struct point *star)
{
    if (star->next)
        free_background(star->next);

    free(star);
}
