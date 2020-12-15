#include "init.h"
#include "utils.h"
#include "menu.h"
#include "event.h"

void achieve_trophy(struct window *window, enum trophies id)
{
    window->trophy.is_unlocking_trophies = 1;

    int i = 0;
    while (window->trophy.id[i] != -1)
        i++;

    if (i == 0)
        window->trophy.is_unlocking_fade_in = 1;

    window->trophy.id[i] = id;

    if (i + 1 < NUM_TROPHIES)
        window->trophy.id[i + 1] = -1;

    window->save->trophies[id] = 1;
}


void render_trophy_pop_up(struct window *window)
{
    SDL_Color white = { 230, 230, 230, 255 };
    SDL_Color blue = { GREEN_R, GREEN_G, GREEN_B, 255 };

    SDL_Rect pos = { .x = DEFAULT_W - window->trophy.offset_x, .y = 20, .w = 750 - 20, .h = 130 };
    SDL_Rect pos_trophy = pos;
    pos_trophy.x += 10;
    pos_trophy.y += 10;
    SDL_QueryTexture(window->img->trophy, NULL, NULL, &pos_trophy.w, &pos_trophy.h);

    resize_pos_for_resolution(window, &pos);
    resize_pos_for_resolution(window, &pos_trophy);

    SDL_SetRenderDrawColor(window->renderer, 160, 160, 160, 195);
    SDL_RenderFillRect(window->renderer, &pos);
    SDL_SetRenderDrawColor(window->renderer, GREEN_R, GREEN_G, GREEN_B, 255);
    SDL_RenderDrawRect(window->renderer, &pos);

    SDL_RenderCopy(window->renderer, window->img->trophy, NULL, &pos_trophy);

    render_text(window, window->fonts->pixel_large, window->txt[TROPHY_1_TITLE + window->trophy.id[0] * 3], white, DEFAULT_W - window->trophy.offset_x + 140, 30);
    render_text(window, window->fonts->pixel, window->txt[TROPHY_1_LINE_1 + window->trophy.id[0] * 3], blue, DEFAULT_W - window->trophy.offset_x + 140, 75);
    render_text(window, window->fonts->pixel, window->txt[TROPHY_1_LINE_2 + window->trophy.id[0] * 3], blue, DEFAULT_W - window->trophy.offset_x + 140, 105);

    if (window->trophy.is_unlocking_fade_in)
    {
        if (window->trophy.offset_x == 0)
            Mix_PlayChannel(-1, window->sounds->trophy, 0);

        window->trophy.offset_x += 20;

        if (window->trophy.offset_x >= 750)
        {
            window->trophy.is_unlocking_fade_in = 0;
            window->trophy.is_unlocking_idleing = 240;
        }
    }
    else if (window->trophy.is_unlocking_idleing)
    {
        window->trophy.is_unlocking_idleing--;

        if (window->trophy.is_unlocking_idleing <= 0)
            window->trophy.is_unlocking_fade_out = 1;
    }
    else if (window->trophy.is_unlocking_fade_out)
    {
        window->trophy.offset_x -= 10;

        if (window->trophy.offset_x <= 0)
        {
            window->trophy.is_unlocking_fade_out = 0;

            for (int i = 0; i < NUM_TROPHIES - 1; i++)
                window->trophy.id[i] = window->trophy.id[i + 1];

            if (window->trophy.id[0] == -1)
                window->trophy.is_unlocking_trophies = 0;
            else
                window->trophy.is_unlocking_fade_in = 1;
        }
    }
}


static void render_view_trophies(struct window *window, Uint32 begin, unsigned selected)
{
    Uint32 alpha = SDL_GetTicks() - begin;

    if (alpha > TITLE_ALPHA_MAX)
        alpha = TITLE_ALPHA_MAX;
    else if (alpha == 0)
        alpha = 1;

    SDL_Color orange = { 255, 127, 39, alpha };

    render_text(window, window->fonts->zero4b_30_small, window->txt[TROPHIES], orange, 150, 150);

    SDL_Color title_color = { GREEN_R, GREEN_G, GREEN_B, alpha };
    SDL_Color blue = { BLUE_R, BLUE_G, BLUE_B, alpha };
    SDL_Color white = { 195, 195, 195, alpha };

    for (int i = 0; i < NUM_TROPHIES; i++)
    {
        render_text(window, window->fonts->pixel_large, window->txt[TROPHY_1_TITLE + i * 3], title_color, 290, 308 + i * 130);
        char s[256] = { 0 };
        strcpy(s, window->txt[TROPHY_1_LINE_1 + i * 3]);
        strcat(s, " ");
        strcat(s, window->txt[TROPHY_1_LINE_2 + i * 3]);
        render_text(window, window->fonts->pixel, s, white, 290, 358 + i * 130);

        if (window->save->trophies[i])
        {
            SDL_Rect pos = { .x = 150, .y = 290 + i * 130, .w = 0, .h = 0 };
            SDL_QueryTexture(window->img->trophy, NULL, NULL, &pos.w, &pos.h);
            resize_pos_for_resolution(window, &pos);
            SDL_RenderCopy(window->renderer, window->img->trophy, NULL, &pos);
        }
    }

    render_text(window, window->fonts->zero4b_30_extra_small, window->txt[BACK_1],
                selected == 1 ? title_color : blue, 150, 880);
}


void view_trophies(struct window *window)
{
    int escape = 0;
    Uint32 begin = SDL_GetTicks();
    unsigned selected = 0;
    SDL_Rect areas[1];
    areas[0].x = 150;
    areas[0].y = 880;
    TTF_SizeText(window->fonts->zero4b_30_extra_small, window->txt[BACK_1], &areas[0].w, &areas[0].h);

    while (!escape)
    {
        update_events(window->in, window, 0);
        handle_quit_event(window, 0);

        escape = handle_escape_event(window) || (selected > 0 && handle_play_event(window));
        handle_select_arrow_event(window, &selected, 1, areas);

        // Display black background
        SDL_SetRenderDrawColor(window->renderer, 8, 8, 8, 255);
        SDL_RenderClear(window->renderer);

        // Process/Draw all the things
        render_stars(window);
        render_view_trophies(window, begin, selected);
        render_controller_input_texts(window, begin, 1);
        SDL_RenderPresent(window->renderer);

        // Wait a frame
        SDL_framerateDelay(window->fps);
    }
}

