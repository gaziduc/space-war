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
                         .y = 32,
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
                         .y = 32,
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
                         .y = 32,
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

    render_text(window, window->fonts->pixel, s, color, 10, 100);
}

static void render_bombs(struct window *window)
{
    char s[50] = { 0 };
    sprintf(s, "BOMBS %d", window->num_bombs);

    SDL_Color color = { .r = 255, .g = 255, .b = 255, .a = 192 };

    render_text(window, window->fonts->pixel, s, color, 10, 70);
}


static void render_ammo(struct window *window, struct player *player, int player_num)
{
    char s[50] = { 0 };

    if (player->ammo >= 0)
        sprintf(s, "AMMO  %d", player->ammo);
    else
        sprintf(s, "AMMO  999+");

    SDL_Color color = { .r = 255, .g = 255, .b = 255, .a = 255 };

    render_text(window, window->fonts->pixel, s, color, 20 + player_num * 220, 32);
}

void render_hud(struct window *window)
{
    render_score(window);
    render_bombs(window);

    static SDL_Color blue = { .r = 0, .g = 128, .b = 255, .a = 255 };
    static SDL_Color red = { .r = 255, .g = 0, .b = 0, .a = 255 };

    Uint32 alpha = SDL_GetTicks() - window->mission_start_time;

    if (alpha > 255)
    {
        if (alpha < 6000)
            alpha = 255;
        else if (alpha <= 6255)
            alpha = -(alpha - 6000) + 255;
        else
            alpha = 0;
    }

    SDL_Color blue_transparent = { .r = 0, .g = 128, .b = 255, .a = alpha };
    SDL_Color red_transparent = { .r = 255, .g = 0, .b = 0, .a = alpha };

    for (unsigned i = 0; i < window->num_players; i++)
    {
        char s[32] = { 0 };
        sprintf(s, "P%d: %s", i + 1, window->is_lan && i == 1 ? "NETWORK"
                                     : window->player[i].input_type == KEYBOARD ? "KEYBOARD"
                                     : window->player[i].input_type == MOUSE ? "MOUSE" : "CONTROLLER");
        render_text(window, window->fonts->pixel_small_bold, s, i == 0 ? blue : red, 10 + i * 220, 10);

        if (alpha > 0)
            render_text(window, window->fonts->pixel_small_bold, s,
                        i == 0 ? blue_transparent : red_transparent,
                        window->player[i].pos.x - 25, window->player[i].pos.y - 25);

        render_life(window, &window->player[i], i);
        render_ammo(window, &window->player[i], i);
    }
}


void set_hud_text(struct list *new, SDL_Rect *pos_dst, struct window *window)
{
    /* Get text width */
    int w = 0;
    TTF_SizeText(window->fonts->pixel, "+100", &w, NULL);

    new->pos_dst.x = pos_dst->x + pos_dst->w / 2 - w / 2;
    new->pos_dst.y = pos_dst->y;
    new->last_time_hurt = SDL_GetTicks();
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
