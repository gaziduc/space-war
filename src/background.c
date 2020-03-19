#include "init.h"
#include "background.h"
#include "utils.h"
#include <stdlib.h>
#include <SDL2/SDL.h>


void init_background(struct window *window)
{
    window->stars = xmalloc(sizeof(struct point), window->window);
    window->stars->next = NULL;

    // Create some initial points
    for (int c = 0; c < 8 * window->h; c++)
    {
        struct point *new = xmalloc(sizeof(struct point), window->window);
        new->x = rand() % window->w;
        new->y = rand() % window->h;
        new->z = (rand() % 5) + 2;
        new->opacity = rand() % 256;
        if (new->z > 4)
            new->opacity /= 2;

        new->next = window->stars->next;
        window->stars->next = new;
    }
}

void move_background(struct window *window, unsigned long framecount)
{
    struct point *last = window->stars;
    struct point *p = window->stars->next;

    while (p)
    {
        if (framecount % p->z == 0)
            p->x--;

        // Delete point
        if (p->x < 0)
        {
            struct point *to_delete = p;
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
        struct point *new = xmalloc(sizeof(struct point), window->window);
        new->x = window->w - 1;
        new->y = rand() % window->h;
        new->z = (rand() % 5) + 2;
        new->opacity = rand() % 256;
        if (new->z > 4)
            new->opacity /= 2;

        new->next = window->stars->next;
        window->stars->next = new;
    }
}


void render_background(struct window *window)
{
    struct point *p = window->stars->next;

    while (p)
    {
        SDL_SetRenderDrawColor(window->renderer, p->opacity, p->opacity, p->opacity, p->opacity);
        SDL_RenderDrawPoint(window->renderer, p->x, p->y);

        p = p->next;
    }
}


void free_background(struct point *star)
{
    if (star->next)
        free_background(star->next);

    free(star);
}
