#include "utils.h"
#include "path.h"
#include "list.h"
#include "object.h"
#include <SDL2/SDL.h>

void set_object_attributes(struct list *new, enum object_type type,
                           struct window *window, struct collision_texture *collision)
{
    new->texture = collision;

    init_position(window->w, POS_CENTERED, window, new->texture->texture, &new->pos_dst);

    new->type = type;
    new->framecount = 0;
}

void create_object(struct window *window, enum object_type type)
{
    list_push_front(NULL, window, OBJECT_LIST, NULL,
                    NULL, type, 0);

    window->paths->index++;
}


void move_objects(struct window *window)
{
    struct list *temp = window->list[OBJECT_LIST]->next;
    struct list *prev = window->list[OBJECT_LIST];

    while (temp)
    {
        if (temp->framecount % 2 == 0)
            temp->pos_dst.x--;

        temp->framecount++;

        // Prevent out of bounds by deleting the object if not on screen
        if (temp->pos_dst.x + temp->pos_dst.w <= 0)
        {
            struct list *to_delete = temp;
            prev->next = temp->next;
            free(to_delete);

            // Go to next shot
            temp = prev->next;
        }
        else
        {
            // Go to next shot
            prev = temp;
            temp = temp->next;
        }
    }
}


void render_objects(struct window *window)
{
    struct list *temp = window->list[OBJECT_LIST]->next;

    while (temp)
    {
        // Display object
        SDL_RenderCopy(window->renderer, temp->texture->texture, NULL, &temp->pos_dst);

        // Go to next object
        temp = temp->next;
    }
}

void render_shield_aura(struct window *window, SDL_Rect *ship_pos)
{
    if (SDL_GetTicks() - window->shield_time < SHIELD_TIME)
    {
        int w = 0;
        int h = 0;
        SDL_QueryTexture(window->img->aura, NULL, NULL, &w, &h);

        SDL_Rect pos = {
                        .x = ship_pos->x + ship_pos->w / 2 - w / 2,
                        .y = ship_pos->y + ship_pos->h / 2 - h / 2,
                        .w = w,
                        .h = h
                       };

        SDL_RenderCopy(window->renderer, window->img->aura, NULL, &pos);
    }
}