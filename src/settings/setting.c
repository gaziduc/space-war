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


int is_fullscreen(struct window *window)
{
    Uint32 flags = SDL_GetWindowFlags(window->window);

    return (flags & SDL_WINDOW_FULLSCREEN) == SDL_WINDOW_FULLSCREEN;
}

static void populate_settings_texts(struct window *window, char s_list[NUM_SETTINGS + NUM_TITLES_SETTINGS + 1][128])
{
    strcpy(s_list[AUDIO - 1], window->txt[AUDIO_TXT]);
    sprintf(s_list[1], window->txt[MUSIC_VOLUME_S], window->settings->music_volume / 8, "----------------", MIX_MAX_VOLUME / 8 - window->settings->music_volume / 8, "                ");
    sprintf(s_list[2], window->txt[SFX_VOLUME_S], window->settings->sfx_volume / 8, "----------------", MIX_MAX_VOLUME / 8 - window->settings->sfx_volume / 8, "                ");
    strcpy(s_list[VIDEO - 1], window->txt[VIDEO_TXT]);
    sprintf(s_list[4], window->txt[FULLSCREEN_S], is_fullscreen(window) ? window->txt[YES] : window->txt[NO]);
    sprintf(s_list[5], window->txt[RESOLUTION_DXD_S], window->w, window->h, window->resolution_index == 0 ? window->txt[NATIVE] : "");
    strcpy(s_list[INPUTS - 1], window->txt[INPUTS_TXT]);

    for (unsigned i = 0; i < MAX_PLAYERS; i++)
    {
        char temp[128] = { 0 };
        sprintf(temp, "%s [C.%d]", window->in->c[window->player[i].controller_num].name, window->player[i].controller_num + 1);
        sprintf(s_list[7 + i], window->txt[P1_INPUT_S + i], window->player[i].input_type == KEYBOARD ? window->txt[KEYBOARD_TXT] :
                                           window->player[i].input_type == MOUSE ? window->txt[MOUSE_TXT] :
                                           temp);
    }

    strcpy(s_list[9], window->txt[KEYBOARD_CONTROLS]);
    sprintf(s_list[10], window->txt[MOUSE_SENSITIVITY_S], window->settings->mouse_sensitivity == 0 ? window->txt[LOW_X1] : window->txt[HIGH_X2]);
    sprintf(s_list[11], window->txt[CONTROLLER_FORCE_FEEDBACK_S], window->settings->is_force_feedback ? window->txt[YES] : window->txt[NO]);
    strcpy(s_list[12], window->txt[BACK_7]);
}


static int get_setting_x_offset(struct window *window, int setting_num)
{
    if ((setting_num == 2 && window->settings->music_volume > 0)
        || (setting_num == 3 && window->settings->sfx_volume > 0)
        || (setting_num == 6 && window->resolution_index > 0)
        || (setting_num == 8 && window->player[0].input_type > 0)
        || (setting_num == 9 && window->player[1].input_type > 0))
    {
        return 60;
    }

    return 0;
}


static int has_setting_x_right_arrow(struct window *window, int setting_num)
{
    if ((setting_num == 2 && window->settings->music_volume < MIX_MAX_VOLUME)
        || (setting_num == 3 && window->settings->sfx_volume < MIX_MAX_VOLUME)
        || (setting_num == 6 && window->resolution_index < NUM_RESOLUTIONS - 1)
        || (setting_num == 8 && (window->player[0].input_type < CONTROLLER || window->player[0].controller_num == 0))
        || (setting_num == 9 && (window->player[1].input_type < CONTROLLER || window->player[1].controller_num == 0)))
    {
        return 1;
    }

    return 0;
}

static void render_settings(struct window *window, Uint32 begin, int selected_item)
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
    SDL_Color yellow = { .r = 255, .g = 255, .b = 0, .a = alpha };

    render_text(window, window->fonts->zero4b_30_small, window->txt[SETTINGS], orange, 150, 150);

    char s_list[NUM_SETTINGS + NUM_TITLES_SETTINGS + 1][128] = { 0 };
    populate_settings_texts(window, s_list);

    int setting_index = 1;

    // Render items
    for (int i = 1; i <= NUM_SETTINGS + NUM_TITLES_SETTINGS + 1; i++)
    {
        if (i == AUDIO || i == VIDEO || i == INPUTS)
        {
            render_text(window, window->fonts->zero4b_30_extra_small, s_list[i - 1], white,
                        150, 280 + (setting_index - 1) * 55 + (i - setting_index) * 30);

            continue;
        }
        else
        {
            int x = i != 13 ? 430 : 150;
            int y = i != 13 ? 280 + (setting_index - 1) * 55 + (i - setting_index - 1) * 30 : 900;
            int x_offset = get_setting_x_offset(window, i);

            int w = 0;
            TTF_SizeText(window->fonts->zero4b_30_extra_small, s_list[i - 1], &w, NULL);

            render_text(window, window->fonts->zero4b_30_extra_small, s_list[i - 1],
                        selected_item != setting_index ? blue : green,
                        x + x_offset, y);

            Uint32 ticks_mod = SDL_GetTicks() % 1000;

            if (x_offset)
                render_text(window, window->fonts->zero4b_30_extra_small, "<", selected_item != setting_index ? yellow : orange, ticks_mod < 500 ? x + ticks_mod / 50 : x + (1000 - ticks_mod) / 50, y);
            if (has_setting_x_right_arrow(window, i))
                render_text(window, window->fonts->zero4b_30_extra_small, " >", selected_item != setting_index ? yellow : orange, ticks_mod < 500 ? x + x_offset + w - ticks_mod / 50 : x + x_offset + w - (1000 - ticks_mod) / 50, y);
        }


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

    sprintf(buffer, "input_type_player_1=%d,%d\n", window->player[0].input_type, window->player[0].controller_num);
    add_string(window, str, buffer);

    sprintf(buffer, "input_type_player_2=%d,%d\n", window->player[1].input_type, window->player[1].controller_num);
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

    add_string(window, str, "\n");
    add_string(window, str, "[misc]\n");

    sprintf(buffer, "show_tutorial=%d\n", window->settings->show_tutorial);
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
        window->settings->is_fullscreen = 0;
        window->settings->music_volume = MIX_MAX_VOLUME / 2;
        window->settings->sfx_volume = MIX_MAX_VOLUME / 2;
        window->settings->is_force_feedback = 1;

#ifndef __EMSCRIPTEN__
        window->resolution_index = 0;
#else
        window->resolution_index = 1; // For performance on browsers => 1280x720 window
#endif

        set_resolution_with_index(window);

        window->player[0].input_type = KEYBOARD;
        window->player[1].input_type = MOUSE;

        window->settings->mouse_sensitivity = 1;

        // Controls
        reset_controls(window);

        window->settings->show_tutorial = 1;

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

    sscanf(str->ptr + index, "input_type_player_1=%d,%d\n", (int *) &window->player[0].input_type, (int *) &window->player[0].controller_num);
    go_to_next_line(&index, str->ptr);

    sscanf(str->ptr + index, "input_type_player_2=%d,%d\n", (int *) &window->player[1].input_type, (int *) &window->player[1].controller_num);
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

    sscanf(str->ptr + index, "\n");
    go_to_next_line(&index, str->ptr);

    sscanf(str->ptr + index, "[misc]\n");
    go_to_next_line(&index, str->ptr);

    sscanf(str->ptr + index, "show_tutorial=%d\n", &window->settings->show_tutorial);
    go_to_next_line(&index, str->ptr);

    free_string(str);

    set_resolution_with_index(window);
}


static int handle_arrow_event(struct window *window, const int selected_item, Uint32 *begin, SDL_Rect *area)
{
    if (window->in->key[SDL_SCANCODE_LEFT]
        || window->in->c[0].button[SDL_CONTROLLER_BUTTON_DPAD_LEFT] || window->in->c[1].button[SDL_CONTROLLER_BUTTON_DPAD_LEFT]
        || (window->in->c[0].axis[SDL_CONTROLLER_AXIS_LEFTX].value <= -DEAD_ZONE
            && window->in->c[0].axis[SDL_CONTROLLER_AXIS_LEFTX].state)
        || (window->in->c[1].axis[SDL_CONTROLLER_AXIS_LEFTX].value <= -DEAD_ZONE
            && window->in->c[1].axis[SDL_CONTROLLER_AXIS_LEFTX].state))
    {
        window->in->key[SDL_SCANCODE_LEFT] = 0;
        window->in->c[0].button[SDL_CONTROLLER_BUTTON_DPAD_LEFT] = 0;
        window->in->c[1].button[SDL_CONTROLLER_BUTTON_DPAD_LEFT] = 0;

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

                    SDL_Rect new_mouse_pos = { .x = area->x + 5, .y = area->y + 5, .w = 3, .h = 3 };
                    resize_pos_for_resolution(window, &new_mouse_pos);
                    SDL_WarpMouseInWindow(window->window, new_mouse_pos.x, new_mouse_pos.y);

                    write_settings(window);
                }
                break;

            case 5:
                if (window->player[0].input_type == CONTROLLER && window->player[0].controller_num > 0)
                    window->player[0].controller_num--;
                else if (window->player[0].input_type > 0)
                    window->player[0].input_type--;
                write_settings(window);
                break;

            case 6:
                if (window->player[1].input_type == CONTROLLER && window->player[1].controller_num > 0)
                    window->player[1].controller_num--;
                else if (window->player[1].input_type > 0)
                    window->player[1].input_type--;
                write_settings(window);
                break;

            default:
                break;
        }
    }

    if (window->in->key[SDL_SCANCODE_RETURN]
        || window->in->key[SDL_SCANCODE_KP_ENTER]
        || window->in->c[0].button[SDL_CONTROLLER_BUTTON_A]
        || window->in->c[1].button[SDL_CONTROLLER_BUTTON_A]
        || window->in->mouse_button[SDL_BUTTON_LEFT])
    {
        window->in->key[SDL_SCANCODE_RETURN] = 0;
        window->in->key[SDL_SCANCODE_KP_ENTER] = 0;
        window->in->c[0].button[SDL_CONTROLLER_BUTTON_A] = 0;
        window->in->c[1].button[SDL_CONTROLLER_BUTTON_A] = 0;
        window->in->mouse_button[SDL_BUTTON_LEFT] = 0;

        switch (selected_item)
        {
            case 3:
                Mix_PlayChannel(-1, window->sounds->play, 0);
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

            case 7:
                Mix_PlayChannel(-1, window->sounds->play, 0);
                controls(window);
                *begin = SDL_GetTicks();
                break;

            case 8:
                Mix_PlayChannel(-1, window->sounds->play, 0);
                window->settings->mouse_sensitivity = !window->settings->mouse_sensitivity;
                write_settings(window);
                break;

            case 9:
                Mix_PlayChannel(-1, window->sounds->play, 0);
                window->settings->is_force_feedback = !window->settings->is_force_feedback;
                write_settings(window);
                break;

            case 10:
                return 0;

            default:
                break;
        }
    }

    if (window->in->key[SDL_SCANCODE_RIGHT]
        || window->in->c[0].button[SDL_CONTROLLER_BUTTON_DPAD_RIGHT] || window->in->c[1].button[SDL_CONTROLLER_BUTTON_DPAD_RIGHT]
        || (window->in->c[0].axis[SDL_CONTROLLER_AXIS_LEFTX].value >= DEAD_ZONE
            && window->in->c[0].axis[SDL_CONTROLLER_AXIS_LEFTX].state)
        || (window->in->c[1].axis[SDL_CONTROLLER_AXIS_LEFTX].value >= DEAD_ZONE
            && window->in->c[1].axis[SDL_CONTROLLER_AXIS_LEFTX].state))
    {
        window->in->key[SDL_SCANCODE_RIGHT] = 0;
        window->in->c[0].button[SDL_CONTROLLER_BUTTON_DPAD_RIGHT] = 0;
        window->in->c[1].button[SDL_CONTROLLER_BUTTON_DPAD_RIGHT] = 0;

        switch (selected_item)
        {
            case 1:
                if (window->settings->music_volume < MIX_MAX_VOLUME)
                    window->settings->music_volume += MIX_MAX_VOLUME / 16; // -= 8

                Mix_VolumeMusic(window->settings->music_volume);
                write_settings(window);
                break;

            case 2:
                if (window->settings->sfx_volume < MIX_MAX_VOLUME)
                    window->settings->sfx_volume += MIX_MAX_VOLUME / 16; // -= 8

                Mix_Volume(-1, window->settings->sfx_volume);
                Mix_PlayChannel(-1, window->sounds->select, 0);
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

                set_resolution_with_index(window);

                SDL_SetWindowSize(window->window, window->w, window->h);
                SDL_SetWindowPosition(window->window,
                                      SDL_WINDOWPOS_CENTERED,
                                      SDL_WINDOWPOS_CENTERED);

                SDL_Rect new_mouse_pos = { .x = area->x + 5, .y = area->y + 5, .w = 3, .h = 3 };
                resize_pos_for_resolution(window, &new_mouse_pos);
                SDL_WarpMouseInWindow(window->window, new_mouse_pos.x, new_mouse_pos.y);

                write_settings(window);
                break;

            case 5:
                if (window->player[0].input_type == CONTROLLER && window->player[0].controller_num < 1)
                    window->player[0].controller_num++;
                else if (window->player[0].input_type < CONTROLLER)
                {
                    window->player[0].input_type++;
                    window->player[0].controller_num = 0;
                }

                write_settings(window);
                break;

            case 6:
               if (window->player[1].input_type == CONTROLLER && window->player[1].controller_num < 1)
                    window->player[1].controller_num++;
                else if (window->player[1].input_type < CONTROLLER)
                {
                    window->player[1].input_type++;
                    window->player[1].controller_num = 0;
                }

                write_settings(window);
                break;

            default:
                break;
        }
    }

    return 1;
}


void settings(struct window *window)
{
    int escape = 0;
    unsigned selected_item = 0;
    Uint32 begin = SDL_GetTicks();
    SDL_Rect areas[NUM_SETTINGS + 1];


    while (!escape)
    {
        // Get and handle events
        update_events(window->in, window, 0);
        handle_quit_event(window, 0);

        char s_list[NUM_SETTINGS + NUM_TITLES_SETTINGS + 1][128] = { 0 };
        populate_settings_texts(window, s_list);

        int setting_index = 0;

        for (int i = 1; i <= NUM_SETTINGS + NUM_TITLES_SETTINGS + 1; i++)
        {
            if (i == AUDIO || i == VIDEO || i == INPUTS)
                continue;

            areas[setting_index].x = i != 13 ? 430 : 150;
            areas[setting_index].y = i != 13 ? 280 + setting_index * 55 + (i - setting_index - 2) * 30 : 900;
            TTF_SizeText(window->fonts->zero4b_30_extra_small, s_list[i - 1], &areas[setting_index].w, &areas[setting_index].h);
            if (i != 13)
                areas[setting_index].w += 120;

            setting_index++;
        }

        handle_select_arrow_event(window, &selected_item, NUM_SETTINGS + 1, areas);

        if (!handle_arrow_event(window, selected_item, &begin, &areas[3]))
            escape = 1;

        escape = escape || handle_escape_event(window);

        // Display black background
        SDL_SetRenderDrawColor(window->renderer, 8, 8, 8, 255);
        SDL_RenderClear(window->renderer);

        // Process/Draw all the things
        render_stars(window);
        render_settings(window, begin, selected_item);
        render_controller_input_texts(window, begin, 1);
        SDL_RenderPresent(window->renderer);

        // Wait a frame
        frame_delay(window->fps);
    }
}
