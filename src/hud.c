#include "init.h"
#include "utils.h"
#include "game.h"
#include "level.h"
#include "hud.h"
#include "menu.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>


static void render_life(struct window *window)
{
    int anim_health_low = window->animated_health_low;
    if (anim_health_low < 0)
        anim_health_low = 0;

    int health = window->health;
    if (health < 0)
        health = 0;

    // Render green part
    boxRGBA(window->renderer, 10, 10, 10 + anim_health_low, 40, 0, 255, 0, 192);

    int anim_health_high = window->animated_health_high;

    // Render orange part
    if (anim_health_high > anim_health_low)
        boxRGBA(window->renderer, 10 + anim_health_low, 10, 10 + anim_health_high, 40, 255, 128, 0, 192);

    // Render red part
    if (anim_health_high < window->max_health)
        boxRGBA(window->renderer, 10 + anim_health_high, 10, 10 + window->max_health, 40, 255, 0, 0, 192);

    if (anim_health_low > health)
        window->animated_health_low--;
    else if (anim_health_low < anim_health_high)
        window->animated_health_high--;

    if (window->lives > 1)
    {
        char s[50] = { 0 };
        sprintf(s, "+%d SHIP(S)", window->lives - 1);

        SDL_Color color = { .r = 255, .g = 255, .b = 255, .a = 192 };
        SDL_Texture *texture = get_text_texture(window, window->fonts->pixel, s, color);

        SDL_Rect pos_dst = { .x = 240, .y = 13, .w = 0, .h = 0 };
        SDL_QueryTexture(texture, NULL, NULL, &pos_dst.w, &pos_dst.h);
        SDL_RenderCopy(window->renderer, texture, NULL, &pos_dst);
        SDL_DestroyTexture(texture);
    }
}

static void render_score(struct window *window)
{
    char s[50] = { 0 };
    sprintf(s, "SCORE: %d", window->score);

    SDL_Color color = { .r = 255, .g = 255, .b = 255, .a = 192 };
    SDL_Texture *texture = get_text_texture(window, window->fonts->pixel, s, color);

    SDL_Rect pos_dst = { .x = 12, .y = 50, .w = 0, .h = 0 };
    SDL_QueryTexture(texture, NULL, NULL, &pos_dst.w, &pos_dst.h);
    SDL_RenderCopy(window->renderer, texture, NULL, &pos_dst);
    SDL_DestroyTexture(texture);
}

static void render_bombs(struct window *window)
{
    char s[50] = { 0 };
    sprintf(s, "BOMBS: %d", window->num_bombs);

    SDL_Color color = { .r = 255, .g = 255, .b = 255, .a = 192 };
    SDL_Texture *texture = get_text_texture(window, window->fonts->pixel, s, color);

    SDL_Rect pos_dst = { .x = 12, .y = 76, .w = 0, .h = 0 };
    SDL_QueryTexture(texture, NULL, NULL, &pos_dst.w, &pos_dst.h);
    SDL_RenderCopy(window->renderer, texture, NULL, &pos_dst);
    SDL_DestroyTexture(texture);
}


void render_hud(struct window *window)
{
    render_life(window);
    render_score(window);
    render_bombs(window);
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
