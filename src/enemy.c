#include "init.h"
#include "utils.h"
#include "enemy.h"
#include "list.h"
#include "game.h"
#include <SDL2/SDL.h>

void set_enemy_pos(struct list *new, struct window *window)
{
    init_position(window->w, POS_CENTERED, window, window->img->enemy, &new->pos_dst);
}


void move_enemies(struct window *window)
{
    struct list *temp = window->list[ENEMY_LIST]->next;
    struct list *prev = window->list[ENEMY_LIST];

    while (temp)
    {
        // Move enemy
        temp->pos_dst.x -= ENEMY_SPEED;
        temp->framecount++;

        if (temp->framecount % FRAMES_BETWEEN_ENEMY_SHOTS == 0)
            list_push_front(&temp->pos_dst, window, ENEMY_SHOT_LIST);

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
        // Display shot
        SDL_RenderCopy(window->renderer, window->img->enemy, NULL, &temp->pos_dst);

        // Go to next shot
        temp = temp->next;
    }
}


void set_enemy_shot_pos(struct list *new, SDL_Rect *pos_dst, struct window *window)
{
    // Setting shot initial position
    int w = 0;
    int h = 0;
    SDL_QueryTexture(window->img->enemy, NULL, NULL, &w, &h);
    new->pos_dst.x = pos_dst->x;
    new->pos_dst.y = pos_dst->y + h / 2;

    SDL_QueryTexture(window->img->shot, NULL, NULL, &new->pos_dst.w, &new->pos_dst.h);
    new->pos_dst.y -= new->pos_dst.h / 2;
}


void move_enemy_shots(struct window *window)
{
    struct list *temp = window->list[ENEMY_SHOT_LIST]->next;
    struct list *prev = window->list[ENEMY_SHOT_LIST];

    while (temp)
    {
        // Move shot
        temp->pos_dst.x -= SHOT_SPEED;

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
        SDL_RenderCopy(window->renderer, window->img->shot, NULL, &temp->pos_dst);

        // Go to next shot
        temp = temp->next;
    }
}

