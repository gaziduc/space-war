#include "list.h"
#include "init.h"
#include "utils.h"
#include "path.h"
#include "boss.h"
#include "enemy.h"
#include <SDL2/SDL.h>

void set_boss_attributes(struct list *new, SDL_Rect *pos,
                         struct window *window, char enemy_type)
{
    new->speed.x = window->paths->data[window->paths->index].line.enemy_path.speed_x;
    new->speed.y = window->paths->data[window->paths->index].line.enemy_path.speed_x;
    new->health = window->paths->data[window->paths->index].line.enemy_path.health;
    new->max_health = new->health;
    new->last_time_hurt = 0;
    new->enemy_type = enemy_type;

    switch (enemy_type)
    {
        case '0':
            new->texture.texture = window->img->boss;
            break;

        case '1':
            new->texture.texture = window->img->final_boss;
            break;

        default:
            break;
    }

    new->rotating = 0;
    new->curr_texture = 0;

    init_position(DEFAULT_W, pos->y, new->texture.texture->texture, &new->pos_dst);
}


void move_boss(struct window *window)
{
    struct list *temp = window->list[BOSS_LIST]->next;

    while (temp)
    {
        // move boss
        if (temp->pos_dst.x + temp->pos_dst.w + 60 > DEFAULT_W)
            temp->pos_dst.x -= temp->speed.x;
        else
        {
            temp->pos_dst.y += temp->speed.y;

            // If vertical out of bounds, change vertical speed
            if (temp->pos_dst.y < 60 || temp->pos_dst.y + temp->pos_dst.h > DEFAULT_H - 60)
                temp->speed.y = -temp->speed.y;
        }

        temp->framecount++;

        if (temp->framecount % FRAMES_BETWEEN_BOSS_SHOTS == 0)
        {
            // Selecting on which player to shoot
            struct player *closest_player = select_player(window, temp);

            list_push_front(&temp->pos_dst, window, ENEMY_SHOT_LIST, NULL,
                            &closest_player->pos, 0, temp->enemy_type);
        }

        temp = temp->next;
    }
}


void render_boss(struct window *window)
{
    struct list *temp = window->list[BOSS_LIST]->next;

    while (temp)
    {
        SDL_Rect pos = { .x = temp->pos_dst.x,
                         .y = temp->pos_dst.y,
                         .w = temp->pos_dst.w,
                         .h = temp->pos_dst.h
                       };

        resize_pos_for_resolution(window, &pos);

        SDL_RenderCopy(window->renderer, temp->texture.texture->texture, NULL, &pos);

        // Go to next boss
        temp = temp->next;
    }
}


void render_boss_health(struct window *window)
{
    struct list *temp = window->list[BOSS_LIST]->next;

    while (temp)
    {
        if (SDL_GetTicks() - temp->last_time_hurt < 1500)
            render_enemy_health(window, temp);

        // Go to next enemy
        temp = temp->next;
    }
}
