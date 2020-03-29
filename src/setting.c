#include "init.h"
#include "event.h"
#include "utils.h"
#include "menu.h"
#include "setting.h"
#include <stdio.h>
#include <SDL2/SDL.h>


static int is_fullscreen(struct window *window)
{
    Uint32 flags = SDL_GetWindowFlags(window->window);

    return (flags & SDL_WINDOW_FULLSCREEN) == SDL_WINDOW_FULLSCREEN;
}


static void render_settings(struct window *window, Uint32 begin, int selected_item)
{
    Uint32 alpha = SDL_GetTicks() - begin;

    if (alpha > TITLE_ALPHA_MAX)
        alpha = TITLE_ALPHA_MAX;
    else if (alpha == 0)
        alpha = 1;

    SDL_Color blue = { .r = 0, .g = 255, .b = 255, .a = alpha };
    SDL_Color green = { .r = 0, .g = 255, .b = 0, .a = alpha };

    char s_list[NUM_SETTINGS][50] = { 0 };
    sprintf(s_list[0], "Fullscreen: %s", is_fullscreen(window) ? "Yes" : "No");
    sprintf(s_list[1], "< Music Volume: %.*s >", window->settings->music_volume / 16, "--------");
    sprintf(s_list[2], "< SFX Volume: %.*s >", window->settings->sfx_volume / 16, "--------");
    sprintf(s_list[3], "Force Feedback: %s", window->settings->is_force_feedback ? "Yes" : "No");
    sprintf(s_list[4], "< Resolution: %dx%d >", window->w, window->h);
    sprintf(s_list[5], "Player 1: %s", window->player[0].is_controller ? "Controller" : "Keyboard");
    sprintf(s_list[6], "Player 2: %s", window->player[1].is_controller ? "Controller" : "Keyboard");


    // Render items
    for (int i = 1; i <= NUM_SETTINGS; i++)
    {
        if (selected_item != i)
            render_text(window, window->fonts->zero4b_30_small, s_list[i - 1], blue,
                        150, 150 + (i - 1) * 100);
        else
            render_text(window, window->fonts->zero4b_30_small, s_list[i - 1], green,
                        150, 150 + (i - 1) * 100);
    }
}


static void write_settings(struct window *window)
{
    // Open file
    FILE *f = fopen("settings.ini", "w");
    if (!f)
        error("settings.ini", "Couldn't open settings.txt for writing.", window->window);

    // Write settings
    fprintf(f, "fullscreen=%d\n", window->settings->is_fullscreen);
    fprintf(f, "music_volume=%d\n", window->settings->music_volume);
    fprintf(f, "sfx_volume=%d\n", window->settings->sfx_volume);
    fprintf(f, "force_feedback=%d\n", window->settings->is_force_feedback);
    fprintf(f, "resolution=%dx%d\n", window->w, window->h);
    fprintf(f, "input_type_player_1=%d\n", window->player[0].is_controller);
    fprintf(f, "input_type_player_2=%d\n", window->player[1].is_controller);

    // Close file
    fclose(f);
}


void load_settings(struct window *window)
{
    window->settings = xmalloc(sizeof(struct settings), window->window);

    // Open file
    FILE *f = fopen("settings.ini", "r");
    if (!f)
    {
        window->settings->is_fullscreen = 1;
        window->settings->music_volume = MIX_MAX_VOLUME;
        window->settings->sfx_volume = MIX_MAX_VOLUME;
        window->settings->is_force_feedback = 1;

        SDL_DisplayMode dm;
        if (SDL_GetDesktopDisplayMode(0, &dm) != 0)
            error("SDL_GetDesktopDisplayMode failed", SDL_GetError(), window->window);

        if (dm.w >= DEFAULT_W && dm.h >= DEFAULT_H)
        {
            window->w = DEFAULT_W;
            window->h = DEFAULT_H;
        }
        else
        {
            window->w = 1280;
            window->h = 720;
        }

        window->player[0].is_controller = 0;
        window->player[1].is_controller = 1;

        return;
    }

    // Read settings
    fscanf(f, "fullscreen=%d\n", &window->settings->is_fullscreen);
    fscanf(f, "music_volume=%d\n", &window->settings->music_volume);
    fscanf(f, "sfx_volume=%d\n", &window->settings->sfx_volume);
    fscanf(f, "force_feedback=%d\n", &window->settings->is_force_feedback);
    fscanf(f, "resolution=%dx%d\n", &window->w, &window->h);
    fscanf(f, "input_type_player_1=%d\n", &window->player[0].is_controller);
    fscanf(f, "input_type_player_2=%d\n", &window->player[1].is_controller);

    // Close file
    fclose(f);
}


static void handle_arrow_event(struct window *window, const int selected_item)
{
    if (window->in->key[SDL_SCANCODE_LEFT]
        || window->in->c.button[SDL_CONTROLLER_BUTTON_DPAD_LEFT]
        || (window->in->c.axis[SDL_CONTROLLER_AXIS_LEFTX].value <= -DEAD_ZONE
            && window->in->c.axis[SDL_CONTROLLER_AXIS_LEFTX].state))
    {
        window->in->key[SDL_SCANCODE_LEFT] = 0;
        window->in->c.button[SDL_CONTROLLER_BUTTON_DPAD_LEFT] = 0;

        switch (selected_item)
        {
            case 2:
                if (window->settings->music_volume > 0)
                {
                    window->settings->music_volume -= MIX_MAX_VOLUME / 8; // -= 16
                    Mix_VolumeMusic(window->settings->music_volume);
                    write_settings(window);
                }
                break;

            case 3:
                if (window->settings->sfx_volume > 0)
                {
                    window->settings->sfx_volume -= MIX_MAX_VOLUME / 8; // -= 16
                    Mix_Volume(-1, window->settings->sfx_volume);
                    Mix_PlayChannel(-1, window->sounds->select, 0);
                    write_settings(window);
                }
                break;

            case 5:
                window->w = 1280;
                window->h = 720;
                SDL_SetWindowSize(window->window, window->w, window->h);
                write_settings(window);
                break;

            default:
                break;
        }
    }

    if (window->in->key[SDL_SCANCODE_RIGHT]
        || window->in->c.button[SDL_CONTROLLER_BUTTON_DPAD_RIGHT]
        || (window->in->c.axis[SDL_CONTROLLER_AXIS_LEFTX].value >= DEAD_ZONE
            && window->in->c.axis[SDL_CONTROLLER_AXIS_LEFTX].state))
    {
        window->in->key[SDL_SCANCODE_RIGHT] = 0;
        window->in->c.button[SDL_CONTROLLER_BUTTON_DPAD_RIGHT] = 0;

        switch (selected_item)
        {
            case 2:
                if (window->settings->music_volume < MIX_MAX_VOLUME)
                {
                    window->settings->music_volume += MIX_MAX_VOLUME / 8; // -= 16
                    Mix_VolumeMusic(window->settings->music_volume);
                    write_settings(window);
                }
                break;

            case 3:
                if (window->settings->sfx_volume < MIX_MAX_VOLUME)
                {
                    window->settings->sfx_volume += MIX_MAX_VOLUME / 8; // -= 16
                    Mix_Volume(-1, window->settings->sfx_volume);
                    Mix_PlayChannel(-1, window->sounds->select, 0);
                    write_settings(window);
                }
                break;

            case 5:
                window->w = 1920;
                window->h = 1080;
                SDL_SetWindowSize(window->window, window->w, window->h);
                write_settings(window);
                break;

            default:
                break;
        }
    }
}


void settings(struct window *window)
{
    int escape = 0;
    int selected_item = 1;
    Uint32 begin = SDL_GetTicks();

    while (!escape)
    {
        // Get and handle events
        update_events(window->in, window);
        handle_quit_event(window, 0);
        handle_select_arrow_event(window, &selected_item, NUM_SETTINGS);

        if (handle_play_event(window))
        {
            switch (selected_item)
            {

                case 1:
                    if (is_fullscreen(window))
                    {
                        window->settings->is_fullscreen = 0;
                        SDL_SetWindowFullscreen(window->window, 0);
                    }
                    else
                    {
                        window->settings->is_fullscreen = 1;
                        SDL_SetWindowFullscreen(window->window, SDL_WINDOW_FULLSCREEN);
                    }
                    break;

                case 4:
                    window->settings->is_force_feedback = !window->settings->is_force_feedback;
                    break;

                case 6:
                case 7:
                    window->player[0].is_controller = !window->player[0].is_controller;
                    window->player[1].is_controller = !window->player[1].is_controller;
                    break;
            }

            write_settings(window);
        }

        handle_arrow_event(window, selected_item);
        escape = handle_escape_event(window);

        // Display black background
        SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
        SDL_RenderClear(window->renderer);

        // Process/Draw all the things
        render_stars(window);
        render_settings(window, begin, selected_item);
        SDL_RenderPresent(window->renderer);

        // Wait a frame
        SDL_framerateDelay(window->fps);
    }
}
