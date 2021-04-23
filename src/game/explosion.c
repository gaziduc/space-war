#include "init.h"
#include "explosion.h"
#include "utils.h"
#include "game.h"
#include <SDL2/SDL.h>


void set_explosion_pos(struct list *new, struct window *window, SDL_FRect *pos_dst, SDL_Texture *texture, int explosion_texture_num)
{
    new->num_explosion = explosion_texture_num;

    int exp_w = 0;
    int exp_h = 0;
    SDL_QueryTexture(window->img->explosion[new->num_explosion], NULL, NULL, &exp_w, &exp_h);

    SDL_Point anim_sprite = { .x = 0, .y = 0 };
    switch (explosion_texture_num)
    {
        case 0:
            anim_sprite.x = 8;
            anim_sprite.y = 8;
            break;

        case 1:
            anim_sprite.x = 5;
            anim_sprite.y = 4;
            break;

        default:
            break;
    }

    // Setting animation to first frame
    new->pos_src.x = 0;
    new->pos_src.y = 0;
    new->pos_src.w = exp_w / anim_sprite.x;
    new->pos_src.h = exp_h / anim_sprite.y;

    int w = 0;
    int h = 0;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);

    new->pos_dst.x = pos_dst->x + w / 2 - new->pos_src.w / 2;
    new->pos_dst.y = pos_dst->y + h / 2 - new->pos_src.h / 2;
    new->pos_dst.w = new->pos_src.w;
    new->pos_dst.h = new->pos_src.h;
}


void move_explosions(struct window *window)
{
    struct list *temp = window->list[EXPLOSION_LIST]->next;
    struct list *prev = window->list[EXPLOSION_LIST];

    int w = 0;
    int h = 0;

    while (temp)
    {
        SDL_QueryTexture(window->img->explosion[temp->num_explosion], NULL, NULL, &w, &h);

        // Increment explosion animation tile
        temp->pos_src.x += temp->pos_src.w;
        if (temp->pos_src.x >= w)
        {
            temp->pos_src.x = 0;
            temp->pos_src.y += temp->pos_src.h;

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
        SDL_Rect pos_dst = { .x = (int) temp->pos_dst.x, .y = (int) temp->pos_dst.y,
                             .w = (int) temp->pos_dst.w, .h = (int) temp->pos_dst.h
                           };

        resize_pos_for_resolution(window, &pos_dst);

        // Display shot
        SDL_RenderCopy(window->renderer, window->img->explosion[temp->num_explosion], &temp->pos_src, &pos_dst);

        // Go to next shot
        temp = temp->next;
    }
}
