#include "list.h"
#include "init.h"
#include "utils.h"
#include "path.h"
#include "boss.h"
#include "enemy.h"
#include <SDL2/SDL.h>

void set_boss_attributes(struct list *new, SDL_Rect *pos, struct window *window)
{
    init_position(window->w, pos->y, window, window->img->boss->texture, &new->pos_dst);

    new->speed.x = window->paths->data[window->paths->index].line.enemy_path.speed_x;
    new->speed.y = 2;
    new->health = window->paths->data[window->paths->index].line.enemy_path.health;
    new->max_health = new->health;
    new->last_time_hurt = 0;
}


void create_boss(struct window *window)
{
    Uint32 ticks = SDL_GetTicks();

    // If it is time to create enemy
    if (ticks - window->last_enemy_time
        >= window->paths->data[window->paths->index].line.enemy_path.time_to_wait)
    {
        int h = 0;
        SDL_QueryTexture(window->img->boss->texture, NULL, NULL, NULL, &h);

        SDL_Rect pos = { .x = 0, .y = window->paths->data[window->paths->index].line.enemy_path.pos_y - h / 2, .w = 0, .h = 0 };

        list_push_front(&pos, window, BOSS_LIST, NULL, NULL, 0);

        window->last_enemy_time = ticks;
        window->paths->index++;
    }
}


void move_boss(struct window *window, SDL_Rect *ship_pos)
{
    struct list *temp = window->list[BOSS_LIST]->next;

    while (temp)
    {
        // move boss
        if (temp->pos_dst.x + temp->pos_dst.w + 60 > window->w)
            temp->pos_dst.x -= temp->speed.x;
        else
        {
            temp->pos_dst.y += temp->speed.y;

            // If vertical out of bounds, change vertical speed
            if (temp->pos_dst.y < 60 || temp->pos_dst.y + temp->pos_dst.h > window->h - 60)
                temp->speed.y = -temp->speed.y;
        }

        temp->framecount++;

        if (temp->framecount % FRAMES_BETWEEN_BOSS_SHOTS == 0)
            list_push_front(&temp->pos_dst, window, ENEMY_SHOT_LIST, NULL, ship_pos, 0);

        temp = temp->next;
    }
}


void render_boss(struct window *window)
{
    struct list *temp = window->list[BOSS_LIST]->next;

    while (temp)
    {
        SDL_RenderCopy(window->renderer, window->img->boss->texture, NULL, &temp->pos_dst);

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
