#include "init.h"
#include "explosion.h"
#include "utils.h"
#include <SDL2/SDL.h>


void set_explosion_pos(struct list *new, SDL_Rect *pos_dst, SDL_Texture *texture)
{
    // Setting animation to first frame
    new->pos_src.x = 0;
    new->pos_src.y = 0;
    new->pos_src.w = EXPLOSION_SIZE;
    new->pos_src.h = EXPLOSION_SIZE;

    // Setting animation position
    int w = 0;
    int h = 0;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    new->pos_dst.x = pos_dst->x + w / 2 - EXPLOSION_SIZE / 2;
    new->pos_dst.y = pos_dst->y + h / 2 - EXPLOSION_SIZE / 2;
    new->pos_dst.w = EXPLOSION_SIZE;
    new->pos_dst.h = EXPLOSION_SIZE;
}


void move_explosions(struct window *window)
{
    struct list *temp = window->list[EXPLOSION_LIST]->next;
    struct list *prev = window->list[EXPLOSION_LIST];

    int w = 0;
    int h = 0;
    SDL_QueryTexture(window->img->explosion, NULL, NULL, &w, &h);

    while (temp)
    {
        // Increment explosion animation tile
        temp->pos_src.x += EXPLOSION_SIZE;
        if (temp->pos_src.x >= w)
        {
            temp->pos_src.x = 0;
            temp->pos_src.y += EXPLOSION_SIZE;

            // Delete explosion when animation is done
            if (temp->pos_src.y >= h)
            {
                struct list *to_delete = temp;
                prev->next = temp->next;
                free(to_delete);

                // Go to next shot
                temp = prev->next;
                continue;
            }
        }

        // Go to next shot
        prev = temp;
        temp = temp->next;
    }
}


void render_explosions(struct window *window)
{
    struct list *temp = window->list[EXPLOSION_LIST]->next;

    while (temp)
    {
        SDL_Rect pos = { .x = temp->pos_dst.x,
                         .y = temp->pos_dst.y,
                         .w = temp->pos_dst.w,
                         .h = temp->pos_dst.h
                       };

        resize_pos_for_resolution(window, &pos);

        // Display shot
        SDL_RenderCopy(window->renderer, window->img->explosion, &temp->pos_src, &pos);

        // Go to next shot
        temp = temp->next;
    }
}
