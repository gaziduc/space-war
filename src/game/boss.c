#include "list.h"
#include "init.h"
#include "utils.h"
#include "path.h"
#include "boss.h"
#include "enemy.h"
#include <SDL2/SDL.h>

void set_boss_attributes(struct list *new, SDL_FRect *pos,
                         struct window *window, char enemy_type, int override, SDL_FRect *speed, int health, int max_health)
{
    new->last_time_hurt = 0;
    new->first_time_hurt = 0;
    new->enemy_type = enemy_type;
    new->texture.texture = window->img->bosses[enemy_type - '0'];
    new->rotating = 0;
    new->curr_texture = 0;


    if (override)
    {
        new->speed.x = speed->x;
        new->speed.y = speed->y;
        new->health = health;
        new->max_health = max_health;
        new->pos_dst.x = pos->x;
        new->pos_dst.y = pos->y;
        new->pos_dst.w = pos->w;
        new->pos_dst.h = pos->h;
    }
    else
    {
        new->speed.x = window->paths->data[window->paths->index].line.enemy_path.speed_x;
        new->speed.y = window->paths->data[window->paths->index].line.enemy_path.speed_x;
        new->health = window->paths->data[window->paths->index].line.enemy_path.health;
        new->max_health = new->health;
        init_position_float(DEFAULT_W, pos->y, new->texture.texture->texture, &new->pos_dst);
    }

    new->framecount = 0;
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

        if ((temp->enemy_type == '0' && temp->framecount % FRAMES_BETWEEN_BOSS_1_SHOTS == 0)
            || (temp->enemy_type == '1' && temp->framecount % FRAMES_BETWEEN_BOSS_2_SHOTS == 0)
            || (temp->enemy_type == '2' && temp->framecount % FRAMES_BETWEEN_BOSS_3_SHOTS == 0)
            || (temp->enemy_type == '3' && temp->framecount % FRAMES_BETWEEN_BOSS_4_SHOTS == 0)
            || (temp->enemy_type == '4' && temp->framecount % FRAMES_BETWEEN_BOSS_5_SHOTS == 0)
            || (temp->enemy_type == '5' && temp->framecount % FRAMES_BETWEEN_BOSS_6_SHOTS == 0)
            || (temp->enemy_type == '6' && temp->framecount % FRAMES_BETWEEN_BOSS_7_SHOTS == 0)
            || (temp->enemy_type == '7' && temp->framecount % FRAMES_BETWEEN_BOSS_8_SHOTS == 0)
            || (temp->enemy_type == '8' && temp->framecount % FRAMES_BETWEEN_BOSS_9_SHOTS == 0)
            || (temp->enemy_type == '9' && temp->framecount % FRAMES_BETWEEN_FINAL_BOSS_SHOTS == 0))
        {
            // Selecting on which player to shoot
            struct player *closest_player = select_player(window, temp);

            list_push_front(&temp->pos_dst, window, ENEMY_SHOT_LIST, NULL,
                            &closest_player->pos, 0, temp->enemy_type, 0);
        }

        temp = temp->next;
    }
}


void render_boss(struct window *window)
{
    struct list *temp = window->list[BOSS_LIST]->next;

    while (temp)
    {
        SDL_Rect pos_dst = { .x = (int) temp->pos_dst.x, .y = (int) temp->pos_dst.y,
                             .w = (int) temp->pos_dst.w, .h = (int) temp->pos_dst.h
                            };

        resize_pos_for_resolution(window, &pos_dst);

        SDL_RenderCopy(window->renderer, temp->texture.texture->texture, NULL, &pos_dst);

        // Go to next boss
        temp = temp->next;
    }
}


void render_boss_health(struct window *window)
{
    struct list *temp = window->list[BOSS_LIST]->next;
    Uint32 ticks = SDL_GetTicks();

    while (temp)
    {
        Uint32 diff_ticks = ticks - temp->last_time_hurt;
        Uint32 first_diff_ticks = ticks - temp->first_time_hurt;

        if (first_diff_ticks < 192)
            render_enemy_health(window, temp, first_diff_ticks);
        else if (diff_ticks < 1500)
            render_enemy_health(window, temp, 192);
        else if (diff_ticks < 1692)
            render_enemy_health(window, temp, 1692 - diff_ticks);


        // Go to next enemy
        temp = temp->next;
    }
}
