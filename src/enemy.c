#include "init.h"
#include "utils.h"
#include "enemy.h"
#include "list.h"
#include "game.h"
#include "path.h"
#include "boss.h"
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

void set_enemy_attributes(struct list *new, SDL_Rect *pos, struct window *window)
{
    init_position(window->w, pos->y, window, window->img->enemy->texture, &new->pos_dst);

    new->speed.x = window->paths->data[window->paths->index].line.enemy_path.speed_x;
    new->health = window->paths->data[window->paths->index].line.enemy_path.health;
    new->max_health = new->health;
    new->last_time_hurt = 0;
}


void create_enemies(struct window *window)
{
    Uint32 ticks = SDL_GetTicks();

    // If it is time to create enemy
    if (ticks - window->last_enemy_time
        >= window->paths->data[window->paths->index].line.enemy_path.time_to_wait)
    {
        int h = 0;

        switch (window->paths->data[window->paths->index].line.enemy_path.enemy_type)
        {
            case 'A':
                SDL_QueryTexture(window->img->enemy->texture, NULL, NULL, NULL, &h);
                break;

            case '0':
                SDL_QueryTexture(window->img->boss->texture, NULL, NULL, NULL, &h);
                break;

            default:
                error("Error in paths file", "Unknown enemy type", window->window);
                break;
        }

        SDL_Rect pos = { .x = 0, .y = window->paths->data[window->paths->index].line.enemy_path.pos_y - h / 2, .w = 0, .h = 0 };

        if (window->paths->data[window->paths->index].line.enemy_path.enemy_type >= 'A'
            && window->paths->data[window->paths->index].line.enemy_path.enemy_type <= 'Z')
            list_push_front(&pos, window, ENEMY_LIST, NULL, NULL);
        else if (window->paths->data[window->paths->index].line.enemy_path.enemy_type >= '0'
            && window->paths->data[window->paths->index].line.enemy_path.enemy_type <= '9')
            list_push_front(&pos, window, BOSS_LIST, NULL, NULL);

        window->last_enemy_time = ticks;
        window->paths->index++;
    }
}


void move_enemies(struct window *window, SDL_Rect *ship_pos)
{
    struct list *temp = window->list[ENEMY_LIST]->next;
    struct list *prev = window->list[ENEMY_LIST];

    while (temp)
    {
        // Move enemy
        temp->pos_dst.x -= temp->speed.x;
        temp->framecount++;

        if (temp->framecount % FRAMES_BETWEEN_ENEMY_SHOTS == 0)
            list_push_front(&temp->pos_dst, window, ENEMY_SHOT_LIST, NULL, ship_pos);

        // Prevent out of bounds by deleting the enemy if not on screen
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

    move_boss(window, ship_pos);
}


void render_enemies(struct window *window)
{
    struct list *temp = window->list[ENEMY_LIST]->next;

    while (temp)
    {
        // Display enemy trail
        render_trail(window, &temp->pos_dst, 1);

        // Display enemy
        SDL_RenderCopy(window->renderer, window->img->enemy->texture, NULL, &temp->pos_dst);

        // Go to next enemy
        temp = temp->next;
    }

    // Display boss if any
    render_boss(window);
}


void render_enemy_health(struct window *window, struct list *enemy)
{
    boxRGBA(window->renderer, enemy->pos_dst.x + enemy->pos_dst.w / 2 - 50, enemy->pos_dst.y - 25,
                              enemy->pos_dst.x + enemy->pos_dst.w / 2 - 50 + (100 * enemy->health) / enemy->max_health, enemy->pos_dst.y - 20,
                              0, 255, 0, 192);

    boxRGBA(window->renderer, enemy->pos_dst.x + enemy->pos_dst.w / 2 - 50 + (100 * enemy->health) / enemy->max_health, enemy->pos_dst.y - 25,
                              enemy->pos_dst.x + enemy->pos_dst.w / 2 + 50, enemy->pos_dst.y - 20,
                              255, 0, 0, 192);
}


void render_enemies_health(struct window *window)
{
    struct list *temp = window->list[ENEMY_LIST]->next;

    while (temp)
    {
        if (SDL_GetTicks() - temp->last_time_hurt < 1500)
            render_enemy_health(window, temp);

        // Go to next enemy
        temp = temp->next;
    }

    render_boss_health(window);
}

void set_enemy_shot_attributes(struct list *new, SDL_Rect *pos_dst, struct window *window,
                               SDL_Rect *ship_pos)
{
    // Setting shot initial position
    int w = 0;
    int h = 0;
    SDL_QueryTexture(window->img->enemy->texture, NULL, NULL, &w, &h);
    new->pos_dst.x = pos_dst->x;
    new->pos_dst.y = pos_dst->y + h / 2;

    SDL_QueryTexture(window->img->enemy_shot->texture, NULL, NULL, &new->pos_dst.w, &new->pos_dst.h);
    new->pos_dst.y -= new->pos_dst.h / 2;
    new->pos_dst.x += new->pos_dst.w;

    // Setting shot speed (horizontal and vertical)
    int gap_x = new->pos_dst.x + new->pos_dst.w / 2 - (ship_pos->x + ship_pos->w / 2);
    int gap_y = ship_pos->y + ship_pos->h / 2 - (new->pos_dst.y + new->pos_dst.h / 2);
    float gap = sqrt(gap_x * gap_x + gap_y * gap_y);

    new->speed.x = (gap_x * ENEMY_SHOT_SPEED) / gap;
    new->speed.y = (gap_y * ENEMY_SHOT_SPEED) / gap;
}


void move_enemy_shots(struct window *window)
{
    struct list *temp = window->list[ENEMY_SHOT_LIST]->next;
    struct list *prev = window->list[ENEMY_SHOT_LIST];

    while (temp)
    {
        // Move shot
        temp->pos_dst.x -= temp->speed.x;
        temp->pos_dst.y += temp->speed.y;

        // Prevent out of bounds by deleting the shot if not on screen
        if (temp->pos_dst.x + temp->pos_dst.w <= 0
            || temp->pos_dst.y + temp->pos_dst.h <= 0
            || temp->pos_dst.y >= window->h)
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

void render_enemy_shots(struct window *window)
{
    struct list *temp = window->list[ENEMY_SHOT_LIST]->next;

    while (temp)
    {
        // Display shot
        SDL_RenderCopy(window->renderer, window->img->enemy_shot->texture, NULL, &temp->pos_dst);

        // Go to next shot
        temp = temp->next;
    }
}
