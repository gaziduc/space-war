#include "utils.h"
#include "path.h"
#include "list.h"
#include "object.h"
#include <SDL2/SDL.h>

void set_object_attributes(struct list *new, enum object_type type,
                           struct collision_texture *collision, SDL_FRect* override_pos)
{
    new->texture.texture = collision;
    new->rotating = 0;

    init_position_float(DEFAULT_W, POS_CENTERED, new->texture.texture->texture, &new->pos_dst);
    if (override_pos)
    {
        new->pos_dst.x = override_pos->x;
        new->pos_dst.y = override_pos->y;
    }

    new->type = type;
    new->framecount = 0;
}


void create_object(struct window *window, enum object_type type)
{
    list_push_front(NULL, window, OBJECT_LIST, NULL,
                    NULL, type, 0, 0);

    window->paths->index++;
}


void move_objects(struct window *window)
{
    struct list *temp = window->list[OBJECT_LIST]->next;
    struct list *prev = window->list[OBJECT_LIST];

    while (temp)
    {
        if (temp->type != GALAXY)
            temp->pos_dst.x--;
        else
            temp->pos_dst.x -= 0.3;
        // temp->framecount++;

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


void render_pre_bg_objects(struct window *window)
{
    struct list *temp = window->list[OBJECT_LIST]->next;

    while (temp)
    {
        if (temp->type == GALAXY) // if galaxy
        {
            SDL_Rect pos = { .x = temp->pos_dst.x + window->shake.x,
                         .y = temp->pos_dst.y + window->shake.y,
                         .w = temp->pos_dst.w,
                         .h = temp->pos_dst.h
                       };

            resize_pos_for_resolution(window, &pos);

            // Display object
            SDL_RenderCopy(window->renderer, temp->texture.texture->texture, NULL, &pos);
        }

        // Go to next object
        temp = temp->next;
    }
}

void render_post_bg_objects(struct window *window)
{
    struct list *temp = window->list[OBJECT_LIST]->next;

    while (temp)
    {
        if (temp->type != GALAXY) // if not galaxy
        {
            SDL_Rect pos = { .x = temp->pos_dst.x + window->shake.x,
                         .y = temp->pos_dst.y + window->shake.y,
                         .w = temp->pos_dst.w,
                         .h = temp->pos_dst.h
                       };

            resize_pos_for_resolution(window, &pos);

            // Display object
            SDL_RenderCopy(window->renderer, temp->texture.texture->texture, NULL, &pos);
        }

        // Go to next object
        temp = temp->next;
    }
}

void render_shield_aura(struct window *window, struct player *player)
{
    if (SDL_GetTicks() - player->shield_time < SHIELD_TIME)
    {
        int w = 0;
        int h = 0;
        SDL_QueryTexture(window->img->aura, NULL, NULL, &w, &h);

        SDL_Rect pos = { .x = player->pos.x + player->pos.w / 2 - w / 2,
                         .y = player->pos.y + player->pos.h / 2 - h / 2,
                         .w = w,
                         .h = h
                       };

        resize_pos_for_resolution(window, &pos);

        SDL_RenderCopy(window->renderer, window->img->aura, NULL, &pos);
    }
}
