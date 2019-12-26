#include "utils.h"
#include "path.h"
#include "init.h"
#include "wave.h"
#include "menu.h"
#include <SDL2/SDL.h>


static void render_title(struct window *window, Uint8 alpha)
{
    SDL_Color fg = { 0, 255, 255, alpha };

    SDL_Texture *texture = get_text_texture(
                           window, window->fonts->zero4b_30_small,
                           window->paths->data[window->paths->index].line.title,
                           fg);

    SDL_Rect pos;
    init_position(POS_CENTERED, 40, window, texture, &pos);

    SDL_RenderCopy(window->renderer, texture, NULL, &pos);

    SDL_DestroyTexture(texture);
}


void render_wave_title(struct window *window)
{
    // While there are still enemies, wait
    if (window->list[ENEMY_LIST]->next != NULL)
        return;

    Uint32 ticks = SDL_GetTicks();

    if (!window->is_wave_title)
    {
        window->wave_title_time = ticks;
        window->is_wave_title = 1;
        return;
    }

    Uint32 diff = ticks - window->wave_title_time;

    if (diff < TITLE_ALPHA_MAX)
        render_title(window, diff);
    else if (diff < TIME_SHOW_TITLE - TITLE_ALPHA_MAX)
        render_title(window, TITLE_ALPHA_MAX);
    else if (diff < TIME_SHOW_TITLE)
        render_title(window, TIME_SHOW_TITLE - diff);
    else
    {
        window->last_enemy_time = ticks;
        window->paths->index++;
        window->is_wave_title = 0;
    }
}
