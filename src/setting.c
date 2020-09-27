#include "init.h"
#include "event.h"
#include "utils.h"
#include "menu.h"
#include "setting.h"
#include "controls.h"
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

    char s_list[NUM_SETTINGS][64] = { 0 };
    sprintf(s_list[0], "Fullscreen: %s", is_fullscreen(window) ? "Yes" : "No");
    sprintf(s_list[1], "< Music Volume: %.*s >", window->settings->music_volume / 16, "--------");
    sprintf(s_list[2], "< SFX Volume: %.*s >", window->settings->sfx_volume / 16, "--------");
    sprintf(s_list[3], "Force Feedback: %s", window->settings->is_force_feedback ? "Yes" : "No");
    sprintf(s_list[4], "< Resolution: %dx%d >", window->w, window->h);
    sprintf(s_list[5], "< P1 Input: %s >", window->player[0].input_type == KEYBOARD ? "Keyboard" :
                                           window->player[0].input_type == MOUSE ? "Mouse" : "Controller");
    sprintf(s_list[6], "< P2 Input: %s >", window->player[1].input_type == KEYBOARD ? "Keyboard" :
                                           window->player[1].input_type == MOUSE ? "Mouse" : "Controller");
    strcpy(s_list[7], "Keyboard Controls...");


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


void write_settings(struct window *window)
{
    // Open file
    FILE *f = fopen("settings.ini", "w");
    if (!f)
        error("settings.ini", "Couldn't open settings.txt for writing.", window->window, window->renderer);

    // Write settings
    fputs("[settings]\n", f);
    fprintf(f, "fullscreen=%d\n", window->settings->is_fullscreen);
    fprintf(f, "music_volume=%d\n", window->settings->music_volume);
    fprintf(f, "sfx_volume=%d\n", window->settings->sfx_volume);
    fprintf(f, "force_feedback=%d\n", window->settings->is_force_feedback);
    fprintf(f, "resolution_index=%d\n", window->resolution_index);
    fprintf(f, "input_type_player_1=%d\n", window->player[0].input_type);
    fprintf(f, "input_type_player_2=%d\n", window->player[1].input_type);
    fputs("\n", f);
    fputs("[controls]\n", f);
    fprintf(f, "up=%d\n", window->settings->controls[UP]);
    fprintf(f, "left=%d\n", window->settings->controls[LEFT]);
    fprintf(f, "down=%d\n", window->settings->controls[DOWN]);
    fprintf(f, "right=%d\n", window->settings->controls[RIGHT]);
    fprintf(f, "shoot=%d\n", window->settings->controls[SHOOT]);
    fprintf(f, "bomb=%d\n", window->settings->controls[BOMB]);

    // Close file
    fclose(f);
}

static void set_resolution_with_index(struct window *window)
{
    window->w = window->resolutions[window->resolution_index].x;
    window->h = window->resolutions[window->resolution_index].y;
}

void load_settings(struct window *window)
{
    window->settings = xmalloc(sizeof(struct settings), window->window, window->renderer);

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
            error("SDL_GetDesktopDisplayMode failed", SDL_GetError(), window->window, window->renderer);

        window->resolution_index = 1;
        while (window->resolution_index < NUM_RESOLUTIONS
            && dm.w >= window->resolutions[window->resolution_index].x
            && dm.h >= window->resolutions[window->resolution_index].y)
        {
            window->resolution_index++;
        }

        window->resolution_index--;
        set_resolution_with_index(window);

        window->player[0].input_type = KEYBOARD;
        window->player[1].input_type = MOUSE;

        // Controls
        window->settings->controls[UP] = SDL_SCANCODE_UP;
        window->settings->controls[LEFT] = SDL_SCANCODE_LEFT;
        window->settings->controls[DOWN] = SDL_SCANCODE_DOWN;
        window->settings->controls[RIGHT] = SDL_SCANCODE_RIGHT;
        window->settings->controls[SHOOT] = SDL_SCANCODE_SPACE;
        window->settings->controls[BOMB] = SDL_SCANCODE_B;

        return;
    }

    // Read settings
    fscanf(f, "[settings]\n");
    fscanf(f, "fullscreen=%d\n", &window->settings->is_fullscreen);
    fscanf(f, "music_volume=%d\n", &window->settings->music_volume);
    fscanf(f, "sfx_volume=%d\n", &window->settings->sfx_volume);
    fscanf(f, "force_feedback=%d\n", &window->settings->is_force_feedback);
    fscanf(f, "resolution_index=%d\n", &window->resolution_index);
    fscanf(f, "input_type_player_1=%d\n", (int *) &window->player[0].input_type);
    fscanf(f, "input_type_player_2=%d\n", (int *) &window->player[1].input_type);
    fscanf(f, "\n");
    fscanf(f, "[controls]\n");
    fscanf(f, "up=%d\n", (int *) &window->settings->controls[UP]);
    fscanf(f, "left=%d\n", (int *) &window->settings->controls[LEFT]);
    fscanf(f, "down=%d\n", (int *) &window->settings->controls[DOWN]);
    fscanf(f, "right=%d\n", (int *) &window->settings->controls[RIGHT]);
    fscanf(f, "shoot=%d\n", (int *) &window->settings->controls[SHOOT]);
    fscanf(f, "bomb=%d\n", (int *) &window->settings->controls[BOMB]);

    set_resolution_with_index(window);

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
                if (window->resolution_index > 0)
                {
                    window->resolution_index--;
                    set_resolution_with_index(window);

                    SDL_SetWindowSize(window->window, window->w, window->h);
                    SDL_SetWindowPosition(window->window,
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED);
                    write_settings(window);
                }
                break;

            case 6:
                if (window->player[0].input_type > 0)
                    window->player[0].input_type--;
                write_settings(window);
                break;

            case 7:
                if (window->player[1].input_type > 0)
                    window->player[1].input_type--;
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
                ;
                SDL_DisplayMode dm;
                if (SDL_GetDesktopDisplayMode(0, &dm) != 0)
                    error("SDL_GetDesktopDisplayMode failed", SDL_GetError(), window->window, window->renderer);

                // Check if resolution is supported
                if (window->resolution_index + 1 < NUM_RESOLUTIONS
                    && dm.w >= window->resolutions[window->resolution_index + 1].x
                    && dm.h >= window->resolutions[window->resolution_index + 1].y)
                {
                    window->resolution_index++;
                    set_resolution_with_index(window);

                    SDL_SetWindowSize(window->window, window->w, window->h);
                    SDL_SetWindowPosition(window->window,
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED);
                    write_settings(window);
                }
                break;

            case 6:
                if (window->player[0].input_type < NUM_INPUT_TYPE - 1)
                    window->player[0].input_type++;
                write_settings(window);
                break;

            case 7:
                if (window->player[1].input_type < NUM_INPUT_TYPE - 1)
                    window->player[1].input_type++;
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
                        SDL_SetWindowPosition(window->window,
                                              SDL_WINDOWPOS_CENTERED,
                                              SDL_WINDOWPOS_CENTERED);
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

                case 8:
                    controls(window);
                    begin = SDL_GetTicks();
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
