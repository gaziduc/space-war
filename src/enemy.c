#include "init.h"
#include "utils.h"
#include "enemy.h"
#include <SDL2/SDL.h>

void set_enemy_pos(struct list *new, struct window *window)
{
    init_position(window->w, POS_CENTERED, window, window->img->enemy, &new->pos_dst);
}


void move_enemies(struct window *window)
{
    struct list *temp = window->list[ENEMY_LIST]->next;
    struct list *prev = window->list[ENEMY_LIST];

    int w = 0;
    SDL_QueryTexture(window->img->enemy, NULL, NULL, &w, NULL);

    while (temp)
    {
        // Move enemy
        temp->pos_dst.x -= ENEMY_SPEED;

        // Prevent out of bounds by deleting the enemy if not on screen
        if (temp->pos_dst.x <= -w)
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
