#include "init.h"
#include "menu.h"
#include "utils.h"
#include "event.h"

static void render_help(struct window *window, Uint32 begin, unsigned selected)
{
    Uint32 alpha = SDL_GetTicks() - begin;

    if (alpha > TITLE_ALPHA_MAX)
        alpha = TITLE_ALPHA_MAX;
    else if (alpha == 0)
        alpha = 1;

    SDL_Color orange = { .r = 255, .g = 127, .b = 39, .a = alpha };
    SDL_Color white = { 195, 195, 195, alpha };
    SDL_Color blue = { .r = BLUE_R, .g = BLUE_G, .b = BLUE_B, .a = alpha };
    SDL_Color green = { .r = GREEN_R, .g = GREEN_G, .b = GREEN_B, .a = alpha };

    render_text(window, window->fonts->zero4b_30_small, window->txt[HELP], orange, 150, 150);

    TTF_SetFontStyle(window->fonts->pixel, TTF_STYLE_UNDERLINE);
    render_text(window, window->fonts->pixel, "Goal: Kill all enemies", white, 150, 300);
    TTF_SetFontStyle(window->fonts->pixel, TTF_STYLE_NORMAL);

    render_text(window, window->fonts->pixel, "You loose if:", white, 150, 380);
    render_text(window, window->fonts->pixel, "  - An enemy pass on the left of the screen", white, 150, 420);
    render_text(window, window->fonts->pixel, "  - Your health points reach 0", white, 150, 460);

    render_text(window, window->fonts->pixel, "You can throw a bomb to destroy all enemies on screen", white, 150, 540);

    render_text(window, window->fonts->pixel, "              | Keyboard (can be modified) |     Mouse     |       Controller", white, 150, 620);
    render_text(window, window->fonts->pixel, "-----------------------------------------------------------------------------------------", white, 150, 660);
    render_text(window, window->fonts->pixel, "Move:         |          Arrows            |   Move mouse  |     Move Left joystick", white, 150, 700);
    render_text(window, window->fonts->pixel, "Shoot:        |          Space             |   Left click  |  RT (second right trigger)", white, 150, 740);
    render_text(window, window->fonts->pixel, "Throw a bomb: |            B               |  Right click  |            X", white, 150, 780);

    render_text(window, window->fonts->zero4b_30_extra_small, window->txt[BACK_1],
                selected == 1 ? green : blue, 150, 880);
}


void help(struct window *window)
{
    int escape = 0;
    unsigned selected = 0;
    Uint32 begin = SDL_GetTicks();
    SDL_Rect areas[1];
    areas[0].x = 150;
    areas[0].y = 880;
    TTF_SizeText(window->fonts->zero4b_30_extra_small, window->txt[BACK_1], &areas[0].w, &areas[0].h);

    while (!escape)
    {
        // Get and handle events
        update_events(window->in, window, 0);
        handle_quit_event(window, 0);

        escape = handle_escape_event(window) || (selected > 0 && handle_play_event(window));
        handle_select_arrow_event(window, &selected, 1, areas);

        // Display black bg
        SDL_SetRenderDrawColor(window->renderer, 8, 8, 8, 255);
        SDL_RenderClear(window->renderer);

        render_stars(window);
        render_help(window, begin, selected);
        render_controller_input_texts(window, begin, 1);
        SDL_RenderPresent(window->renderer);

        SDL_framerateDelay(window->fps);
    }
}
