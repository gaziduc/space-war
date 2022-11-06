#include "init.h"
#include "utils.h"
#include "game.h"
#include "level.h"
#include "hud.h"
#include "menu.h"
#include "shot.h"
#include "path.h"
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

        SDL_Rect pos = { .x = 20 + player_num * 220,
                         .y = 39,
                         .w = anim_health_low,
                         .h = 25
                       };

        resize_pos_for_resolution(window, &pos);
        SDL_RenderFillRect(window->renderer, &pos);
    }

    int anim_health_high = player->animated_health_high;

    // Render orange part
    if (anim_health_high > anim_health_low)
    {
        SDL_SetRenderDrawColor(window->renderer, 255, 128, 0, 192);

        SDL_Rect pos = { .x = 20 + anim_health_low + player_num * 220,
                         .y = 39,
                         .w = anim_health_high - anim_health_low,
                         .h = 25
                       };

        resize_pos_for_resolution(window, &pos);
        SDL_RenderFillRect(window->renderer, &pos);
    }

    // Render red part
    if (anim_health_high < window->max_health)
    {
        SDL_SetRenderDrawColor(window->renderer, 255, 0, 0, 192);

        SDL_Rect pos = { .x = 20 + anim_health_high + player_num * 220,
                         .y = 39,
                         .w = window->max_health - anim_health_high,
                         .h = 25
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
        player->animated_health_high -= 2;
}

static void render_score(struct window *window)
{
    char s[50] = { 0 };
    sprintf(s, window->txt[SCORE_D], window->score);

    SDL_Color color = { .r = 255, .g = 255, .b = 255, .a = 234 };

    render_text(window, window->fonts->craft_large, s, color, 20, 168);
}

static void render_bombs(struct window *window)
{
    int w = 0;
    int h = 0;
    SDL_QueryTexture(window->img->bomb, NULL, NULL, &w, &h);

    for (int i = 0; i < window->num_bombs; i++)
    {
        SDL_Rect pos = { .x = 20 + i * 80, .y = 107, .w = w, .h = h };
        resize_pos_for_resolution(window, &pos);
        SDL_RenderCopy(window->renderer, window->img->bomb, NULL, &pos);
    }
}


static void render_combo(struct window *window)
{
    Uint32 ticks = SDL_GetTicks();

    if (window->combo >= 2)
    {
        char s[50] = { 0 };
        sprintf(s, "%d COMBO", window->combo);

        Uint32 offset = ticks - window->combo_time;
        SDL_Color orange = { .r = 255, .g = 127, .b = 39, .a = offset < 224 ? offset : 224 };

        render_text(window, window->fonts->craft_large, s, orange, 20, offset < 224 ? 284 - offset / 10 : 262);
    }

    Uint32 delay = ticks - window->last_combo_time;

    if (delay < 2224)
    {
        char s[50] = { 0 };
        sprintf(s, "+%d [%d COMBO]", compute_combo_score(window->last_combo), window->last_combo);

        SDL_Color yellow = { .r = 255, .g = 255, .b = 0, .a = delay < 224 ? delay : delay < 2000 ? 224 : 2224 - delay };

        render_text(window, window->fonts->craft_large, s, yellow, 20, 222);
    }
}

static void render_ammo(struct window *window, struct player *player, int player_num)
{
    char s[50] = { 0 };

    if (player->ammo >= 0)
        sprintf(s, window->txt[AMMO_D], player->ammo);
    else
        sprintf(s, window->txt[AMMO_999]);

    SDL_Color color = { .r = 255, .g = 255, .b = 255, .a = 255 };

    render_text(window, window->fonts->craft, s, color, 20 + player_num * 220, 70);
}

void render_hud(struct window *window)
{
    render_score(window);
    render_bombs(window);
    render_combo(window);

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
        char temp[32] = { 0 };
        sprintf(temp, "CONTR. %d", window->player[i].controller_num + 1);
        sprintf(s, "P%d: %s", i + 1, window->is_lan && i == 1 ? window->txt[NETWORK]
                                     : window->player[i].input_type == KEYBOARD ? window->txt[KEYBOARD_TXT]
                                     : window->player[i].input_type == MOUSE ? window->txt[MOUSE_TXT]
                                     : temp);

        render_text(window, window->fonts->craft_small, s, i == 0 ? blue : red, 20 + i * 220, 15);

        if (alpha > 0)
            render_text(window, window->fonts->craft_small, s,
                        i == 0 ? blue_transparent : red_transparent,
                        window->player[i].pos.x - 15, window->player[i].pos.y - 25);

        render_life(window, &window->player[i], i);
        render_ammo(window, &window->player[i], i);
    }
}


void set_hud_text(struct list *new, SDL_FRect *pos_dst, struct window *window, int override_pos_and_time, Uint32 last_time_hurt)
{
    if (override_pos_and_time)
    {
        new->pos_dst.x = pos_dst->x;
        new->pos_dst.y = pos_dst->y;
        new->pos_dst.w = pos_dst->w;
        new->pos_dst.h = pos_dst->h;
        new->last_time_hurt = last_time_hurt;
    }
    else
    {
        /* Get text width */
        int w = 0;
        TTF_SizeText(window->fonts->craft, "+100", &w, NULL);

        new->pos_dst.x = pos_dst->x + pos_dst->w / 2 - w / 2;
        new->pos_dst.y = pos_dst->y;
        new->last_time_hurt = SDL_GetTicks();
    }
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

        SDL_Color blue = { .r = BLUE_R, .g = BLUE_G, .b = BLUE_B, .a = alpha };
        render_text(window, window->fonts->craft, "+100", blue,
                    temp->pos_dst.x, temp->pos_dst.y);

        // Go to next element
        temp = temp->next;
    }
}


