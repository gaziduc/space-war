#include "init.h"
#include "stars.h"
#include "utils.h"
#include <SDL2/SDL.h>


void init_background(struct window *window)
{
    window->stars = xmalloc(sizeof(struct points), window->window);
    window->stars->next = NULL;
}

void move_background(struct window *window)
{
    struct points *last = window->stars;
    struct points *p = window->stars->next;

    while (p)
    {
        p->x -= p->z / 48;

        // Delete point
        if (p->x < 0)
        {
            struct points *to_delete = p;
            last->next = p->next;
            p = p->next;
            free(to_delete);
        }
        else
        {
            last = p;
            p = p->next;
        }
    }

    // Create some points
    for (int c = 0; c < rand() % 6; c++)
    {
        struct points *new = xmalloc(sizeof(struct points), window->window);
        new->x = window->w - 1;
        new->y = rand() % window->h;
        new->z = rand() % 256;

        new->next = window->stars->next;
        window->stars->next = new;
    }
}


void render_background(struct window *window)
{
    struct points *p = window->stars->next;

    while (p)
    {
        SDL_SetRenderDrawColor(window->renderer, p->z, p->z, p->z, p->z);
        SDL_RenderDrawPoint(window->renderer, p->x, p->y);

        p = p->next;
    }
}


void free_background(struct points *star)
{
    if (star->next)
        free_background(star->next);

    free(star);
}
