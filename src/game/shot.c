#include "init.h"
#include "game.h"
#include "utils.h"
#include "weapon.h"
#include <SDL2/SDL.h>

void set_shot_pos(struct list *new, SDL_Rect *pos_dst, struct window *window)
{
    // Setting shot initial position
    int w = 0;
    int h = 0;
    SDL_QueryTexture(window->img->ship->texture, NULL, NULL, &w, &h);
    new->pos_dst.x = pos_dst->x + w;
    new->pos_dst.y = pos_dst->y + h / 2;

    SDL_QueryTexture(window->img->shot[window->weapon]->texture, NULL, NULL,
                     &new->pos_dst.w, &new->pos_dst.h);

    new->pos_dst.x -= new->pos_dst.w;
    new->pos_dst.y -= new->pos_dst.h / 2;
}


void move_shots(struct window *window)
{
    struct list *temp = window->list[MY_SHOTS_LIST]->next;
    struct list *prev = window->list[MY_SHOTS_LIST];

    while (temp)
    {
        // Move shot
        temp->pos_dst.x += get_weapon_speed(window->weapon);

        // Prevent out of bounds by deleting the shot if not on screen
        if (temp->pos_dst.x >= DEFAULT_W)
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


void render_shots(struct window *window)
{
    struct list *temp = window->list[MY_SHOTS_LIST]->next;

    while (temp)
    {
        SDL_Rect pos = { .x = temp->pos_dst.x,
                         .y = temp->pos_dst.y,
                         .w = temp->pos_dst.w,
                         .h = temp->pos_dst.h
                       };

        resize_pos_for_resolution(window, &pos);

        // Display shot
        SDL_RenderCopy(window->renderer, window->img->shot[window->weapon]->texture, NULL, &pos);

        // Go to next shot
        temp = temp->next;
    }
}
