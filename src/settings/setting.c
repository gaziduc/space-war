#include "init.h"
#include "event.h"
#include "utils.h"
#include "menu.h"
#include "setting.h"
#include "controls.h"
#include "string_vec.h"
#include "file.h"
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
    SDL_Color orange = { .r = 255, .g = 128, .b = 0, .a = alpha };
    SDL_Color white = { .r = 255, .g = 255, .b = 255, .a = alpha };

    render_text(window, window->fonts->zero4b_30_small, "SETTINGS", orange, 150, 150);

    char s_list[NUM_SETTINGS + NUM_TITLES_SETTINGS][64] = { 0 };
    strcpy(s_list[AUDIO - 1], "Audio:");
    sprintf(s_list[1], "< Music Volume: %.*s >", window->settings->music_volume / 8, "----------------");
    sprintf(s_list[2], "< SFX Volume: %.*s >", window->settings->sfx_volume / 8, "----------------");
    strcpy(s_list[VIDEO - 1], "Video:");
    sprintf(s_list[4], "Fullscreen: %s", is_fullscreen(window) ? "Yes" : "No");
    sprintf(s_list[5], "< Resolution: %dx%d %s>", window->w, window->h, window->resolution_index == 0 ? "(Native) " : "");
    strcpy(s_list[INPUTS - 1], "Inputs:");
    sprintf(s_list[7], "< P1 Input: %s >", window->player[0].input_type == KEYBOARD ? "Keyboard" :
                                           window->player[0].input_type == MOUSE ? "Mouse" :
                                           window->player[0].input_type == CONTROLLER ? "Controller" : "Touch screen");
    sprintf(s_list[8], "< P2 Input: %s >", window->player[1].input_type == KEYBOARD ? "Keyboard" :
                                           window->player[1].input_type == MOUSE ? "Mouse" :
                                           window->player[1].input_type == CONTROLLER ? "Controller" : "Touch screen");
    strcpy(s_list[9], "Keyboard Controls...");
    sprintf(s_list[10], "Mouse Sensitivity: %s", window->settings->mouse_sensitivity == 0 ? "Low (x1)" : "High (x2)");
    sprintf(s_list[11], "Controller Force Feedback: %s", window->settings->is_force_feedback ? "Yes" : "No");

    int setting_index = 1;

    // Render items
    for (int i = 1; i <= NUM_SETTINGS + NUM_TITLES_SETTINGS; i++)
    {
        if (i == AUDIO || i == VIDEO || i == INPUTS)
        {
            render_text(window, window->fonts->zero4b_30_extra_small, s_list[i - 1], white,
                        150, 280 + (i - 1) * 55);

            setting_index--;
        }
        else if (selected_item != setting_index)
            render_text(window, window->fonts->zero4b_30_extra_small, s_list[i - 1], blue,
                        200, 280 + (i - 1) * 55);
        else
            render_text(window, window->fonts->zero4b_30_extra_small, s_list[i - 1], green,
                        200, 280 + (i - 1) * 55);

        setting_index++;
    }
}


void write_settings(struct window *window)
{
    struct string_vec *str = create_string(window);
    char buffer[256] = { 0 };

    // AUDIO
    add_string(window, str, "[audio]\n");

    sprintf(buffer, "music_volume=%d\n", window->settings->music_volume);
    add_string(window, str, buffer);

    sprintf(buffer, "sfx_volume=%d\n", window->settings->sfx_volume);
    add_string(window, str, buffer);


    // VIDEO
    add_string(window, str, "\n");
    add_string(window, str, "[video]\n");

    sprintf(buffer, "fullscreen=%d\n", window->settings->is_fullscreen);
    add_string(window, str, buffer);

    sprintf(buffer, "resolution_index=%d\n", window->resolution_index);
    add_string(window, str, buffer);


    // INPUTS
    add_string(window, str, "\n");
    add_string(window, str, "[inputs]\n");

    sprintf(buffer, "input_type_player_1=%d\n", window->player[0].input_type);
    add_string(window, str, buffer);

    sprintf(buffer, "input_type_player_2=%d\n", window->player[1].input_type);
    add_string(window, str, buffer);

    sprintf(buffer, "mouse_sensitivity=%d\n", window->settings->mouse_sensitivity);
    add_string(window, str, buffer);

    sprintf(buffer, "controller_force_feedback=%d\n", window->settings->is_force_feedback);
    add_string(window, str, buffer);


    // KEYBOARD
    add_string(window, str, "\n");
    add_string(window, str, "[keyboard_controls]\n");

    sprintf(buffer, "up=%d\n", window->settings->controls[UP]);
    add_string(window, str, buffer);

    sprintf(buffer, "left=%d\n", window->settings->controls[LEFT]);
    add_string(window, str, buffer);

    sprintf(buffer, "down=%d\n", window->settings->controls[DOWN]);
    add_string(window, str, buffer);

    sprintf(buffer, "right=%d\n", window->settings->controls[RIGHT]);
    add_string(window, str, buffer);

    sprintf(buffer, "shoot=%d\n", window->settings->controls[SHOOT]);
    add_string(window, str, buffer);

    sprintf(buffer, "bomb=%d\n", window->settings->controls[BOMB]);
    add_string(window, str, buffer);


    // Open file
    SDL_RWops *f = SDL_RWFromFile("settings.ini", "w");
    if (!f)
        error("settings.ini", "Couldn't open settings.ini for writing.", window->window, window->renderer);

    SDL_RWwrite(f, str->ptr, 1, strlen(str->ptr));

    // Close file
    SDL_RWclose(f);
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
    struct string_vec *str = dump_file_in_string("settings.ini", window);
    if (!str)
    {
        #ifndef __APPLE__
            window->settings->is_fullscreen = 1;
        #else
            window->settings->is_fullscreen = 0;
        #endif

        window->settings->music_volume = MIX_MAX_VOLUME;
        window->settings->sfx_volume = MIX_MAX_VOLUME;
        window->settings->is_force_feedback = 1;

        window->resolution_index = 0;
        set_resolution_with_index(window);

        window->player[0].input_type = KEYBOARD;
        window->player[1].input_type = MOUSE;

        window->settings->mouse_sensitivity = 1;

        // Controls
        reset_controls(window);

        return;
    }

    size_t index = 0;

    // Read settings
    sscanf(str->ptr + index, "[audio]\n");
    go_to_next_line(&index, str->ptr);

    sscanf(str->ptr + index, "music_volume=%d\n", &window->settings->music_volume);
    go_to_next_line(&index, str->ptr);

    sscanf(str->ptr + index, "sfx_volume=%d\n", &window->settings->sfx_volume);
    go_to_next_line(&index, str->ptr);

    sscanf(str->ptr + index, "\n");
    go_to_next_line(&index, str->ptr);

    sscanf(str->ptr + index, "[video]\n");
    go_to_next_line(&index, str->ptr);

    sscanf(str->ptr + index, "fullscreen=%d\n", &window->settings->is_fullscreen);
    go_to_next_line(&index, str->ptr);

    sscanf(str->ptr + index, "resolution_index=%d\n", &window->resolution_index);
    go_to_next_line(&index, str->ptr);

    sscanf(str->ptr + index, "\n");
    go_to_next_line(&index, str->ptr);

    sscanf(str->ptr + index, "[inputs]\n");
    go_to_next_line(&index, str->ptr);

    sscanf(str->ptr + index, "input_type_player_1=%d\n", (int *) &window->player[0].input_type);
    go_to_next_line(&index, str->ptr);

    sscanf(str->ptr + index, "input_type_player_2=%d\n", (int *) &window->player[1].input_type);
    go_to_next_line(&index, str->ptr);

    sscanf(str->ptr + index, "mouse_sensitivity=%d\n", &window->settings->mouse_sensitivity);
    go_to_next_line(&index, str->ptr);

    sscanf(str->ptr + index, "controller_force_feedback=%d\n", &window->settings->is_force_feedback);
    go_to_next_line(&index, str->ptr);

    sscanf(str->ptr + index, "\n");
    go_to_next_line(&index, str->ptr);

    sscanf(str->ptr + index, "[keyboard_controls]\n");
    go_to_next_line(&index, str->ptr);

    sscanf(str->ptr + index, "up=%d\n", (int *) &window->settings->controls[UP]);
    go_to_next_line(&index, str->ptr);

    sscanf(str->ptr + index, "left=%d\n", (int *) &window->settings->controls[LEFT]);
    go_to_next_line(&index, str->ptr);

    sscanf(str->ptr + index, "down=%d\n", (int *) &window->settings->controls[DOWN]);
    go_to_next_line(&index, str->ptr);

    sscanf(str->ptr + index, "right=%d\n", (int *) &window->settings->controls[RIGHT]);
    go_to_next_line(&index, str->ptr);

    sscanf(str->ptr + index, "shoot=%d\n", (int *) &window->settings->controls[SHOOT]);
    go_to_next_line(&index, str->ptr);

    sscanf(str->ptr + index, "bomb=%d\n", (int *) &window->settings->controls[BOMB]);
    go_to_next_line(&index, str->ptr);


    set_resolution_with_index(window);
}


static void handle_arrow_event(struct window *window, const int selected_item, Uint32 *begin)
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
            case 1:
                if (window->settings->music_volume > 0)
                {
                    window->settings->music_volume -= MIX_MAX_VOLUME / 16; // -= 8
                    Mix_VolumeMusic(window->settings->music_volume);
                    write_settings(window);
                }
                break;

            case 2:
                if (window->settings->sfx_volume > 0)
                {
                    window->settings->sfx_volume -= MIX_MAX_VOLUME / 16; // -= 8
                    Mix_Volume(-1, window->settings->sfx_volume);
                    Mix_PlayChannel(-1, window->sounds->select, 0);
                    write_settings(window);
                }
                break;

            case 4:
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

            case 5:
                if (window->player[0].input_type > 0)
                    window->player[0].input_type--;
                write_settings(window);
                break;

            case 6:
                if (window->player[1].input_type > 0)
                    window->player[1].input_type--;
                write_settings(window);
                break;

            default:
                break;
        }
    }

    if (window->in->key[SDL_SCANCODE_RETURN]
        || window->in->key[SDL_SCANCODE_KP_ENTER]
        || window->in->c.button[SDL_CONTROLLER_BUTTON_A]
        || window->in->mouse_button[SDL_BUTTON_LEFT]
        // Enter or right
        || window->in->key[SDL_SCANCODE_RIGHT]
        || window->in->c.button[SDL_CONTROLLER_BUTTON_DPAD_RIGHT]
        || (window->in->c.axis[SDL_CONTROLLER_AXIS_LEFTX].value >= DEAD_ZONE
            && window->in->c.axis[SDL_CONTROLLER_AXIS_LEFTX].state))
    {
        window->in->key[SDL_SCANCODE_RETURN] = 0;
        window->in->key[SDL_SCANCODE_KP_ENTER] = 0;
        window->in->c.button[SDL_CONTROLLER_BUTTON_A] = 0;
        window->in->mouse_button[SDL_BUTTON_LEFT] = 0;
        window->in->key[SDL_SCANCODE_RIGHT] = 0;
        window->in->c.button[SDL_CONTROLLER_BUTTON_DPAD_RIGHT] = 0;

        switch (selected_item)
        {
            case 1:
                if (window->settings->music_volume < MIX_MAX_VOLUME)
                    window->settings->music_volume += MIX_MAX_VOLUME / 16; // -= 8
                else
                    window->settings->music_volume = 0;

                Mix_VolumeMusic(window->settings->music_volume);
                write_settings(window);
                break;

            case 2:
                if (window->settings->sfx_volume < MIX_MAX_VOLUME)
                    window->settings->sfx_volume += MIX_MAX_VOLUME / 16; // -= 8
                else
                    window->settings->sfx_volume = 0;

                Mix_Volume(-1, window->settings->sfx_volume);
                Mix_PlayChannel(-1, window->sounds->select, 0);
                write_settings(window);
                break;

            case 3:
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
                write_settings(window);
                break;

            case 4:
                ;
                SDL_DisplayMode dm;
                if (SDL_GetDesktopDisplayMode(0, &dm) != 0)
                    error("SDL_GetDesktopDisplayMode failed", SDL_GetError(), window->window, window->renderer);

                // Check if resolution is supported
                if (window->resolution_index + 1 < NUM_RESOLUTIONS
                    && dm.w >= window->resolutions[window->resolution_index + 1].x
                    && dm.h >= window->resolutions[window->resolution_index + 1].y)
                    window->resolution_index++;
                else
                    window->resolution_index = 0;

                set_resolution_with_index(window);

                SDL_SetWindowSize(window->window, window->w, window->h);
                SDL_SetWindowPosition(window->window,
                                      SDL_WINDOWPOS_CENTERED,
                                      SDL_WINDOWPOS_CENTERED);
                write_settings(window);
                break;

            case 5:
                if (window->player[0].input_type < NUM_INPUT_TYPE - 1)
                    window->player[0].input_type++;
                else
                    window->player[0].input_type = 0;

                write_settings(window);
                break;

            case 6:
                if (window->player[1].input_type < NUM_INPUT_TYPE - 1)
                    window->player[1].input_type++;
                else
                    window->player[1].input_type = 0;

                write_settings(window);
                break;

            case 7:
                controls(window);
                *begin = SDL_GetTicks();
                break;

            case 8:
                window->settings->mouse_sensitivity = !window->settings->mouse_sensitivity;
                write_settings(window);
                break;

            case 9:
                window->settings->is_force_feedback = !window->settings->is_force_feedback;
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
    unsigned selected_item = 0;
    Uint32 begin = SDL_GetTicks();
    SDL_Rect areas[NUM_SETTINGS];

    for (unsigned i = 0; i < 2; i++)
    {
        areas[i].x = 200;
        areas[i].y = 280 + (i + 1) * 55;
        areas[i].w = 1500;
        areas[i].h = 55;
    }

    for (unsigned i = 2; i < 4; i++)
    {
        areas[i].x = 200;
        areas[i].y = 280 + (i + 2) * 55;
        areas[i].w = 1500;
        areas[i].h = 55;
    }

    for (unsigned i = 4; i < 9; i++)
    {
        areas[i].x = 200;
        areas[i].y = 280 + (i + 3) * 55;
        areas[i].w = 1500;
        areas[i].h = 55;
    }


    while (!escape)
    {
        // Get and handle events
        update_events(window->in, window);
        handle_quit_event(window, 0);
        handle_select_arrow_event(window, &selected_item, NUM_SETTINGS, areas);

        handle_arrow_event(window, selected_item, &begin);
        escape = handle_escape_event(window);

        // Display black background
        SDL_SetRenderDrawColor(window->renderer, 8, 8, 8, 255);
        SDL_RenderClear(window->renderer);

        // Process/Draw all the things
        render_stars(window);
        render_settings(window, begin, selected_item);
        SDL_RenderPresent(window->renderer);

        // Wait a frame
        SDL_framerateDelay(window->fps);
    }
}
