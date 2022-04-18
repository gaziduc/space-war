#include "event.h"
#include "init.h"
#include "utils.h"
#include "game.h"
#include "stars.h"
#include "menu.h"
#include "level.h"
#include "setting.h"
#include "credits.h"
#include "version.h"
#include "help.h"
#include "trophies.h"
#include <stdlib.h>
#include <SDL2/SDL.h>


void render_controller_input_texts(struct window *window, Uint32 begin, int display_back)
{
    if (window->in->last_input_type == CONTROLLER)
    {
        Uint32 alpha = SDL_GetTicks() - begin;

        if (alpha > TITLE_ALPHA_MAX)
            alpha = TITLE_ALPHA_MAX;
        else if (alpha == 0)
            alpha = 1;

        SDL_Color white = { 220, 220, 220, alpha };

        SDL_Rect pos_a = { .x = 1430, .y = 150, .w = 0, .h = 0 };
        SDL_QueryTexture(window->img->a_button, NULL, NULL, &pos_a.w, &pos_a.h);
        resize_pos_for_resolution(window, &pos_a);

        SDL_SetTextureAlphaMod(window->img->a_button, alpha);
        SDL_RenderCopy(window->renderer, window->img->a_button, NULL, &pos_a);
        render_text(window, window->fonts->zero4b_30_extra_small, window->txt[SELECT], white, 1500, 154);

        if (display_back)
        {
            SDL_Rect pos_b = { .x = 1430, .y = 210, .w = 0, .h = 0 };
            SDL_QueryTexture(window->img->b_button, NULL, NULL, &pos_b.w, &pos_b.h);
            resize_pos_for_resolution(window, &pos_b);

            SDL_SetTextureAlphaMod(window->img->b_button, alpha);
            SDL_RenderCopy(window->renderer, window->img->b_button, NULL, &pos_b);
            render_text(window, window->fonts->zero4b_30_extra_small, window->txt[BACK_LOWERCASE], white, 1500, 214);
        }
    }
}


static void render_menu_texts(struct window *window, Uint32 begin, int selected_item)
{
    Uint32 alpha = SDL_GetTicks() - begin;

    if (alpha > TITLE_ALPHA_MAX)
        alpha = TITLE_ALPHA_MAX;
    else if (alpha == 0)
        alpha = 1;

    SDL_Color blue = { .r = BLUE_R, .g = BLUE_G, .b = BLUE_B, .a = alpha };
    SDL_Color green = { .r = GREEN_R, .g = GREEN_G, .b = GREEN_B, .a = alpha };
    SDL_Color orange = { 255, 127, 39, alpha };
    SDL_Color white = { .r = 195, .g = 195, .b = 195, .a = alpha };

    // Render title
    render_text(window, window->fonts->zero4b_30, "SPACE WAR", orange, 150, 150);

    // Render items
    char *s_list[NUM_ITEMS] = { window->txt[PLAY], window->txt[SETTINGS], window->txt[HELP], window->txt[TROPHIES], window->txt[CREDITS], window->txt[QUIT] };

    for (int i = 1; i <= NUM_ITEMS; i++)
    {
        render_text(window, window->fonts->zero4b_30_small, s_list[i - 1],
                    selected_item != i ? blue : green,
                    250, 370 + (i - 1) * 100);
    }

    SDL_Rect pos = { .x = 150, .y = 365, .w = 0, .h = 0 };
    SDL_QueryTexture(window->img->play_icon, NULL, NULL, &pos.w, &pos.h);
    resize_pos_for_resolution(window, &pos);
    SDL_SetTextureAlphaMod(window->img->play_icon, alpha);
    SDL_RenderCopy(window->renderer, window->img->play_icon, NULL, &pos);

    pos.x = 150;
    pos.y = 465;
    SDL_QueryTexture(window->img->wheel, NULL, NULL, &pos.w, &pos.h);
    resize_pos_for_resolution(window, &pos);
    SDL_SetTextureAlphaMod(window->img->wheel, alpha);
    SDL_RenderCopy(window->renderer, window->img->wheel, NULL, &pos);

    pos.x = 150;
    pos.y = 565;
    SDL_QueryTexture(window->img->help, NULL, NULL, &pos.w, &pos.h);
    resize_pos_for_resolution(window, &pos);
    SDL_SetTextureAlphaMod(window->img->help, alpha);
    SDL_RenderCopy(window->renderer, window->img->help, NULL, &pos);

    pos.x = 150;
    pos.y = 665;
    pos.w = 64;
    pos.h = 64;
    resize_pos_for_resolution(window, &pos);
    SDL_SetTextureAlphaMod(window->img->trophy, alpha);
    SDL_RenderCopy(window->renderer, window->img->trophy, NULL, &pos);

    pos.x = 150;
    pos.y = 765;
    SDL_QueryTexture(window->img->copyright, NULL, NULL, &pos.w, &pos.h);
    resize_pos_for_resolution(window, &pos);
    SDL_SetTextureAlphaMod(window->img->copyright, alpha);
    SDL_RenderCopy(window->renderer, window->img->copyright, NULL, &pos);

    pos.x = 150;
    pos.y = 865;
    SDL_QueryTexture(window->img->exit_logo, NULL, NULL, &pos.w, &pos.h);
    resize_pos_for_resolution(window, &pos);
    SDL_SetTextureAlphaMod(window->img->exit_logo, alpha);
    SDL_RenderCopy(window->renderer, window->img->exit_logo, NULL, &pos);

    render_text(window, window->fonts->zero4b_30_small, VERSION_INGAME, white, 1300, 870);
}



void render_stars(struct window *window)
{
    for (;;)
    {
        struct return_point rp;
        int return_code = process_point(window->universe, &rp, window);

        if (return_code == 0)
            break;

        if (return_code == 1)
        {
            SDL_SetRenderDrawColor(window->renderer,
                                   220 * rp.opacity / OPACITY_MAX,
                                   220 * rp.opacity / OPACITY_MAX,
                                   220 * rp.opacity / OPACITY_MAX,
                                   220 * rp.opacity / OPACITY_MAX);

            if (rp.size > 1)
            {
                SDL_Rect pos = { .x = rp.x + window->w / 2 - 1,
                                 .y = -rp.y + window->h / 2 - 1,
                                 .w = rp.size,
                                 .h = rp.size
                               };

                SDL_RenderFillRect(window->renderer, &pos);
            }
            else
                SDL_RenderDrawPoint(window->renderer, rp.x + window->w / 2 - 1, -rp.y + window->h / 2 - 1);
        }
    }

#ifndef __EMSCRIPTEN__
    int num_points = 64;
#else
    int num_points = 16;
#endif

    for (int c = 0; c < rand() % num_points; c++)
        new_point(window->universe, window);

    if (window->trophy.is_unlocking_trophies)
        render_trophy_pop_up(window);
}


void menu(struct window *window)
{
    load_music_and_play(window, "data/endgame.ogg", 1);

    int escape = 0;
    unsigned selected_item = 0;
    Uint32 begin = SDL_GetTicks();
    SDL_Rect areas[NUM_ITEMS];

    for (unsigned i = 0; i < NUM_ITEMS; i++)
    {
        areas[i].x = 150;
        areas[i].y = 370 + i * 100;
        TTF_SizeText(window->fonts->zero4b_30_small, window->txt[0 + i], &areas[i].w, &areas[i].h);
        areas[i].w += 100;
    }

    while (!escape)
    {
        // Get and handle events
        update_events(window->in, window, 0);
        handle_quit_event(window, 0);
        handle_select_arrow_event(window, &selected_item, NUM_ITEMS, areas);

        if (selected_item > 0 && handle_play_event(window))
        {
            switch (selected_item)
            {
                case 1:
                    select_num_players(window);
                    begin = SDL_GetTicks();
                    break;
                case 2:
                    settings(window);
                    begin = SDL_GetTicks();
                    break;
                case 3:
                    help(window);
                    begin = SDL_GetTicks();
                    break;
                case 4:
                    view_trophies(window);
                    begin = SDL_GetTicks();
                    break;
                case 5:
                    credits(window);
                    begin = SDL_GetTicks();
                    break;
                case 6:
                    escape = 1;
                    break;
            }
        }

        // Display black background
        SDL_SetRenderDrawColor(window->renderer, 8, 8, 8, 255);
        SDL_RenderClear(window->renderer);

        // Process/Draw all the things
        render_stars(window);
        render_menu_texts(window, begin, selected_item);
        render_controller_input_texts(window, begin, 0);
        SDL_RenderPresent(window->renderer);

        // Wait a frame
        frame_delay(window->fps);
    }
}

static void render_num_players(struct window *window, Uint32 begin, int selected_item)
{
    Uint32 alpha = SDL_GetTicks() - begin;

    if (alpha > TITLE_ALPHA_MAX)
        alpha = TITLE_ALPHA_MAX;
    else if (alpha == 0)
        alpha = 1;

    SDL_Color blue = { .r = BLUE_R, .g = BLUE_G, .b = BLUE_B, .a = alpha };
    SDL_Color green = { .r = GREEN_R, .g = GREEN_G, .b = GREEN_B, .a = alpha };
    SDL_Color orange = { 255, 127, 39, alpha };

    // Render title
    render_text(window, window->fonts->zero4b_30_small, window->txt[SELECT_MODE], orange, 150, 150);

    // Render items
    char *s_list[MAX_PLAYERS + 1] = { window->txt[ONE_PLAYER], window->txt[TWO_PLAYERS_LOCAL], window->txt[BACK_1] };

    for (int i = 1; i <= MAX_PLAYERS + 1; i++)
    {
        render_text(window, window->fonts->zero4b_30_small, s_list[i - 1],
                    selected_item != i ? blue : green,
                    150, 670 + (i - 1) * 100);
    }
}


void select_num_players(struct window *window)
{
    int escape = 0;
    window->num_players = 0;
    Uint32 begin = SDL_GetTicks();
    SDL_Rect areas[MAX_PLAYERS + 1];

    for (unsigned i = 0; i < MAX_PLAYERS + 1; i++)
    {
        areas[i].x = 150;
        areas[i].y = 670 + i * 100;
        TTF_SizeText(window->fonts->zero4b_30_small, window->txt[ONE_PLAYER + i], &areas[i].w, &areas[i].h);
    }

    while (!escape)
    {
        // Get and handle events
        update_events(window->in, window, 0);
        handle_quit_event(window, 0);
        handle_select_arrow_event(window, &window->num_players, MAX_PLAYERS + 2, areas);
        escape = handle_escape_event(window);

        if (window->num_players > 0 && handle_play_event(window))
        {
            if (window->num_players <= MAX_PLAYERS)
            {
                select_level(window);
                begin = SDL_GetTicks();
            }
            else if (window->num_players == MAX_PLAYERS + 1)
                escape = 1;
        }

        // Display black background
        SDL_SetRenderDrawColor(window->renderer, 8, 8, 8, 255);
        SDL_RenderClear(window->renderer);

        // Process/Draw all the things
        render_stars(window);
        render_num_players(window, begin, window->num_players);
        render_controller_input_texts(window, begin, 1);
        SDL_RenderPresent(window->renderer);

        // Wait a frame
        frame_delay(window->fps);
    }
}
