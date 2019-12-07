#include "init.h"
#include "utils.h"
#include "enemy.h"
#include "list.h"
#include "game.h"
#include "path.h"
#include <SDL2/SDL.h>

void set_enemy_pos(struct list *new, SDL_Rect *pos, struct window *window)
{
    init_position(window->w, pos->y, window, window->img->enemy->texture, &new->pos_dst);
}


void set_enemy_speed(struct list *new, struct window *window)
{
    new->speed_x = window->paths->data[window->paths->index].speed_x;
}


void create_enemies(struct window *window)
{
    Uint32 ticks = SDL_GetTicks();

    // If there is at least one more enemy to create and if it is time to create it
    if (window->paths->index < window->paths->size
        && ticks - window->last_enemy_time >= window->paths->data[window->paths->index].time_to_wait)
    {
        int h = 0;
        SDL_QueryTexture(window->img->enemy->texture, NULL, NULL, NULL, &h);

        SDL_Rect pos = { .x = 0, .y = window->paths->data[window->paths->index].pos_y - h / 2, .w = 0, .h = 0 };

        list_push_front(&pos, window, ENEMY_LIST, NULL);

        window->last_enemy_time = ticks;
        window->paths->index++;
    }
}


void move_enemies(struct window *window)
{
    struct list *temp = window->list[ENEMY_LIST]->next;
    struct list *prev = window->list[ENEMY_LIST];

    while (temp)
    {
        // Move enemy
        temp->pos_dst.x -= temp->speed_x;
        temp->framecount++;

        if (temp->framecount % FRAMES_BETWEEN_ENEMY_SHOTS == 0)
            list_push_front(&temp->pos_dst, window, ENEMY_SHOT_LIST, NULL);

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
}


void set_enemy_shot_pos(struct list *new, SDL_Rect *pos_dst, struct window *window)
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
}


void move_enemy_shots(struct window *window)
{
    struct list *temp = window->list[ENEMY_SHOT_LIST]->next;
    struct list *prev = window->list[ENEMY_SHOT_LIST];

    while (temp)
    {
        // Move shot
        temp->pos_dst.x -= ENEMY_SHOT_SPEED;

        // Prevent out of bounds by deleting the shot if not on screen
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
