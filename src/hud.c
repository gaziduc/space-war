#include "init.h"
#include "utils.h"
#include <SDL2/SDL2_gfxPrimitives.h>


static void render_life(struct window *window)
{
    roundedBoxRGBA(window->renderer, 10, 10, window->health, 40, 3, 0, 255, 0, 192);
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

void render_hud(struct window *window)
{
    render_life(window);
    render_score(window);
}
