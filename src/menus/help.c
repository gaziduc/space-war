#include "init.h"
#include "menu.h"
#include "utils.h"
#include "event.h"
#include <stdio.h>

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

    TTF_SetFontStyle(window->fonts->craft, TTF_STYLE_UNDERLINE);
    render_text(window, window->fonts->craft, "Goal: Kill all enemies", white, 150, 280);
    TTF_SetFontStyle(window->fonts->craft, TTF_STYLE_NORMAL);

    render_text(window, window->fonts->craft, "You loose if:", white, 150, 360);
    render_text(window, window->fonts->craft, "  - An enemy pass on the left of the screen", white, 150, 400);
    render_text(window, window->fonts->craft, "  - Your health points reach 0", white, 150, 440);

    render_text(window, window->fonts->craft, "You can throw a bomb to destroy all enemies on screen", white, 150, 520);

    render_text(window, window->fonts->craft, "                        Keyboard [can be modified]                Mouse                               Controller", blue, 150, 600);
    render_text(window, window->fonts->craft, "-----------------------------------------------------------------------------------------------------------------------------------", white, 150, 640);
    render_text(window, window->fonts->craft, "Move:                                Arrows                             Move mouse                   Move Left joystick", white, 150, 680);
    render_text(window, window->fonts->craft, "Shoot:                                Space                               Left click                  RT (second right trigger)", white, 150, 720);
    render_text(window, window->fonts->craft, "use bomb:                             B                                   Right click                                  X", white, 150, 760);

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


void tutorial_help(struct window *window, int tutorial_num)
{
    int escape = 0;
    Uint32 begin = SDL_GetTicks();

    char s[256] = { 0 };
    if (tutorial_num == 1)
        sprintf(s, "Press %s, %s, %s or %s to move.",
                SDL_GetScancodeName(window->settings->controls[UP]),
                SDL_GetScancodeName(window->settings->controls[LEFT]),
                SDL_GetScancodeName(window->settings->controls[DOWN]),
                SDL_GetScancodeName(window->settings->controls[RIGHT]));
    else if (tutorial_num == 2)
        sprintf(s, "Press %s to shoot.", SDL_GetScancodeName(window->settings->controls[SHOOT]));
    else if (tutorial_num == 3)
        sprintf(s, "Press %s to use bomb.", SDL_GetScancodeName(window->settings->controls[BOMB]));

    SDL_Texture *bg = NULL;

    if (!window->in->focus_lost)
    {
        // take a screenshot of background
        SDL_Surface *sshot = SDL_CreateRGBSurface(0, window->w, window->h, 32, 0, 0, 0, 0);
        SDL_RenderReadPixels(window->renderer, NULL, sshot->format->format, sshot->pixels, sshot->pitch);
        bg = SDL_CreateTextureFromSurface(window->renderer, sshot);
        SDL_FreeSurface(sshot);
    }

    while (!escape)
    {
        // Get and handle events
        update_events(window->in, window, 0);
        handle_quit_event(window, 0);

        if (tutorial_num == 1)
        {
            if (window->in->key[window->settings->controls[UP]]
                || window->in->key[window->settings->controls[LEFT]]
                || window->in->key[window->settings->controls[DOWN]]
                || window->in->key[window->settings->controls[RIGHT]])
                    escape = 1;
        }
        else if (tutorial_num == 2)
        {
            if (window->in->key[window->settings->controls[SHOOT]])
                escape = 1;
        }
        else if (tutorial_num == 3)
        {
             if (window->in->key[window->settings->controls[BOMB]])
                escape = 1;
        }

        SDL_SetRenderDrawColor(window->renderer, 8, 8, 8, 255);
        SDL_RenderClear(window->renderer);

        // Copy screenshot on background and add transparency
        if (bg)
            SDL_RenderCopy(window->renderer, bg, NULL, NULL);

        Uint32 alpha = SDL_GetTicks() - begin;

        if (alpha > TITLE_ALPHA_MAX)
            alpha = TITLE_ALPHA_MAX;
        else if (alpha == 0)
            alpha = 1;

        SDL_SetRenderDrawColor(window->renderer, 255, 255, 255, alpha);
        SDL_Color orange = { 255, 128, 32, alpha };

        SDL_Rect pos_line_1 = { .x = 0, .y = 780, .w = DEFAULT_W, .h = 3 };
        resize_pos_for_resolution(window, &pos_line_1);
        SDL_RenderFillRect(window->renderer, &pos_line_1);

        SDL_Rect pos_line_2 = { .x = 0, .y = 840, .w = DEFAULT_W, .h = 3 };
        resize_pos_for_resolution(window, &pos_line_2);
        SDL_RenderFillRect(window->renderer, &pos_line_2);

        render_text(window, window->fonts->craft, s, orange, POS_CENTERED, 800);

        SDL_RenderPresent(window->renderer);
    }
}

