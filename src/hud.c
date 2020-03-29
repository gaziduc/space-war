#include "init.h"
#include "utils.h"
#include "game.h"
#include "level.h"
#include "hud.h"
#include "menu.h"
#include <stdio.h>
#include <SDL2/SDL_ttf.h>


static void render_life(struct window *window, struct player *player, int player_num)
{
    int anim_health_low = player->animated_health_low;
    if (anim_health_low < 0)
        anim_health_low = 0;

    int health = player->health;
    if (health < 0)
        health = 0;

    // Render green part
    if (anim_health_low > 0)
    {
        SDL_SetRenderDrawColor(window->renderer, 0, 255, 0, 192);

        SDL_Rect pos = { .x = 10 + player_num * 220,
                         .y = 10,
                         .w = anim_health_low,
                         .h = 30
                       };

        resize_pos_for_resolution(window, &pos);
        SDL_RenderFillRect(window->renderer, &pos);
    }

    int anim_health_high = player->animated_health_high;

    // Render orange part
    if (anim_health_high > anim_health_low)
    {
        SDL_SetRenderDrawColor(window->renderer, 255, 128, 0, 192);

        SDL_Rect pos = { .x = 10 + anim_health_low + player_num * 220,
                         .y = 10,
                         .w = anim_health_high - anim_health_low,
                         .h = 30
                       };

        resize_pos_for_resolution(window, &pos);
        SDL_RenderFillRect(window->renderer, &pos);
    }

    // Render red part
    if (anim_health_high < window->max_health)
    {
        SDL_SetRenderDrawColor(window->renderer, 255, 0, 0, 192);

        SDL_Rect pos = { .x = 10 + anim_health_high + player_num * 220,
                         .y = 10,
                         .w = window->max_health - anim_health_high,
                         .h = 30
                       };

        resize_pos_for_resolution(window, &pos);
        SDL_RenderFillRect(window->renderer, &pos);
    }

    // If ship took a health potion
    if (anim_health_low < health || anim_health_high < health)
    {
        player->animated_health_low = health;
        player->animated_health_high = health;
    }
    else if (anim_health_low > health)
        player->animated_health_low -= 2;
    else if (anim_health_low < anim_health_high)
        player->animated_health_high--;

    // Not used in the game
    if (player->lives > 1)
    {
        char s[50] = { 0 };
        sprintf(s, "+%d SHIP(S)", player->lives - 1);

        SDL_Color color = { .r = 255, .g = 255, .b = 255, .a = 192 };

        render_text(window, window->fonts->pixel, s, color, 240, 13);
    }
}

static void render_score(struct window *window)
{
    char s[50] = { 0 };
    sprintf(s, "SCORE %d", window->score);

    SDL_Color color = { .r = 255, .g = 255, .b = 255, .a = 192 };

    render_text(window, window->fonts->pixel, s, color, 10, 80);
}

static void render_bombs(struct window *window)
{
    char s[50] = { 0 };
    sprintf(s, "BOMBS %d", window->num_bombs);

    SDL_Color color = { .r = 255, .g = 255, .b = 255, .a = 192 };

    render_text(window, window->fonts->pixel, s, color, 10, 50);
}


static void render_ammo(struct window *window, struct player *player, int player_num)
{
    char s[50] = { 0 };

    if (player->ammo > 0)
        sprintf(s, "AMMO  %d", player->ammo);
    else
        sprintf(s, "AMMO  999+");

    SDL_Color color = { .r = 255, .g = 255, .b = 255, .a = 255 };

    render_text(window, window->fonts->pixel, s, color, 20 + player_num * 220, 10);
}

void render_hud(struct window *window)
{
    render_score(window);
    render_bombs(window);

    for (int i = 0; i < window->num_players; i++)
    {
        render_life(window, &window->player[i], i);
        render_ammo(window, &window->player[i], i);
    }
}


void set_hud_text(struct list *new, SDL_Rect *pos_dst, struct window *window)
{
    /* Get text width */
    // Random color as we are just getting text length
    SDL_Color color = { 0, 0, 0, 0 };
    SDL_Surface *surface = TTF_RenderText_Blended(window->fonts->pixel, "+100", color);

    new->pos_dst.x = pos_dst->x + pos_dst->w / 2 - surface->w / 2;
    new->pos_dst.y = pos_dst->y;
    new->last_time_hurt = SDL_GetTicks();

    SDL_FreeSurface(surface);
}


void move_hud_texts(struct window *window)
{
    struct list *temp = window->list[HUD_LIST]->next;
    struct list *prev = window->list[HUD_LIST];

    while (temp)
    {
        // Delete text if time since it exists > SCORE_TIME
        if (SDL_GetTicks() - temp->last_time_hurt > SCORE_TIME)
        {
            struct list *to_delete = temp;
            prev->next = temp->next;
            free(to_delete);

            temp = prev->next;
        }
        else
        {
            temp->pos_dst.y--;

            prev = temp;
            temp = temp->next;
        }
    }
}


void render_hud_texts(struct window *window)
{
    struct list *temp = window->list[HUD_LIST]->next;

    while (temp)
    {
        // Display text
        Uint32 alpha = SDL_GetTicks() - temp->last_time_hurt;

        if (alpha >= SCORE_TIME)
        {
            // Go to next element
            temp = temp->next;
            continue;
        }

        if (alpha == 0)
            alpha = 1;
        if (alpha > SCORE_TIME - TITLE_ALPHA_MAX)
            alpha = SCORE_TIME - alpha;
        else if (alpha > TITLE_ALPHA_MAX)
            alpha = TITLE_ALPHA_MAX;

        SDL_Color blue = { 0, 255, 255, alpha };
        render_text(window, window->fonts->pixel, "+100", blue,
                    temp->pos_dst.x, temp->pos_dst.y);

        // Go to next element
        temp = temp->next;
    }
}
