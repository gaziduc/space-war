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
    FILE *f = fopen("settings.txt", "w");
    if (!f)
        error("settings.txt", "Couldn't open settings.txt for writing.", window->window);

    // Write settings
    fprintf(f, "fullscreen %d\n", window->settings->is_fullscreen);
    fprintf(f, "music_volume %d\n", window->settings->music_volume);
    fprintf(f, "sfx_volume %d\n", window->settings->sfx_volume);

    // Close file
    fclose(f);
}


void load_settings(struct window *window)
{
    window->settings = xmalloc(sizeof(struct settings), window->window);

    // Open file
    FILE *f = fopen("settings.txt", "r");
    if (!f)
    {
        window->settings->is_fullscreen = 1;
        window->settings->music_volume = MIX_MAX_VOLUME;
        window->settings->sfx_volume = MIX_MAX_VOLUME;

        return;
    }

    // Read settings
    fscanf(f, "fullscreen %d\n", &window->settings->is_fullscreen);
    fscanf(f, "music_volume %d\n", &window->settings->music_volume);
    fscanf(f, "sfx_volume %d\n", &window->settings->sfx_volume);

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

        if (handle_play_event(window) && selected_item == 1)
        {
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