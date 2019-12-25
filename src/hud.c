#include "init.h"
#include "utils.h"
#include "game.h"
#include "level.h"
#include <SDL2/SDL2_gfxPrimitives.h>


static void render_life(struct window *window, int difficulty)
{
    int health = window->health;
    if (health < 0)
        health = 0;

    // Render green part
    boxRGBA(window->renderer, 10, 10, 10 + health, 40, 0, 255, 0, 192);

    // Render red part
    switch (difficulty)
    {
        case EASY:
            if (health < MAX_HEALTH_EASY)
                boxRGBA(window->renderer, 10 + health, 10, 10 + MAX_HEALTH_EASY, 40, 255, 0, 0, 192);
            break;

        case HARD:
            if (health < MAX_HEALTH_HARD)
                boxRGBA(window->renderer, 10 + health, 10, 10 + MAX_HEALTH_HARD, 40, 255, 0, 0, 192);
            break;

        default:
            error("Unknown difficulty level", "Unknown difficulty level", window->window);
            break;
    }

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

void render_hud(struct window *window, int difficulty)
{
    render_life(window, difficulty);
    render_score(window);
    render_bombs(window);
}
