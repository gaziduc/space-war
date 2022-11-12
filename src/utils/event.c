#include "event.h"
#include "init.h"
#include "free.h"
#include "pause.h"
#include "setting.h"
#include <string.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

void update_events(struct input *in, struct window *window, int is_in_level)
{
    in->quit = 0;
    memset(in->text, '\0', sizeof(in->text));
    SDL_Event event;

    // Reset axis state
    for (unsigned controller_num = 0; controller_num < 2; controller_num++)
        for (int i = 0; i < SDL_CONTROLLER_AXIS_MAX; i++)
            in->c[controller_num].axis[i].state = 0;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        // Quit event
        case SDL_QUIT:
            in->quit = 1;
            break;

        // Text input events
        case SDL_TEXTINPUT:
            ; // Ugly but avoid error
            size_t curr_len = strlen(in->text);
            size_t to_add_len = strlen(event.text.text);
            for (size_t i = curr_len; i < sizeof(in->text) - 1; i++)
            {
                if (i - curr_len >= to_add_len)
                    break;

                in->text[i] = event.text.text[i - curr_len];
            }
            /* Last char is already at '\0' */
            break;

        // Keyboard events
        case SDL_KEYDOWN:
            in->key[event.key.keysym.scancode] = 1;
            in->last_input_type = KEYBOARD;
            break;
        case SDL_KEYUP:
            in->key[event.key.keysym.scancode] = 0;
            break;

        // Controller events
        case SDL_CONTROLLERDEVICEADDED:
            if (SDL_IsGameController(event.cdevice.which) && in->num_controllers < 2)
            {
                for (unsigned i = 0; i < 2; i++)
                {
                    if (!in->c[i].controller)
                    {
                        init_controller(in, i, event.cdevice.which);
                        in->last_input_type = CONTROLLER;

                        in->num_controllers++;
                        break;
                    }
                }
            }
            break;

        case SDL_CONTROLLERDEVICEREMOVED:
            for (unsigned i = 0; i < 2; i++)
            {
                if (event.cdevice.which == in->c[i].id)
                {
                    // Free haptic
                    if (in->c[i].haptic)
                    {
                        SDL_HapticClose(in->c[i].haptic);
                        in->c[i].haptic = NULL;
                    }

                    SDL_GameControllerClose(in->c[i].controller);
                    in->c[i].controller = NULL;
                    in->c[i].name = NULL;

                    in->last_input_type = KEYBOARD;

                    in->num_controllers--;
                    break;
                }
            }
            break;

        case SDL_CONTROLLERBUTTONDOWN:
            for (unsigned i = 0; i < 2; i++)
            {
                if (event.cdevice.which == in->c[i].id)
                {
                    in->c[i].button[event.cbutton.button] = 1;
                    break;
                }
            }

            in->last_input_type = CONTROLLER;
            break;

        case SDL_CONTROLLERBUTTONUP:
            for (unsigned i = 0; i < 2; i++)
            {
                if (event.cdevice.which == in->c[i].id)
                {
                    in->c[i].button[event.cbutton.button] = 0;
                    break;
                }
            }

            break;

        case SDL_CONTROLLERAXISMOTION:
            for (unsigned i = 0; i < 2; i++)
            {
                if (event.cdevice.which == in->c[i].id)
                {
                    // For menus
                    if (abs(event.caxis.value) >= MENU_DEAD_ZONE
                        && abs(in->c[i].axis[event.caxis.axis].value) < MENU_DEAD_ZONE)
                        in->c[i].axis[event.caxis.axis].state = 1;

                    in->c[i].axis[event.caxis.axis].value = event.caxis.value;
                    in->last_input_type = CONTROLLER;
                    break;
                }
            }
            break;

        // Handling window event to prevent bug on Windows
        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
                in->focus_lost = 1;
            else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
                in->focus_lost = 0;
            break;

        /* Mouse events */
        case SDL_MOUSEMOTION:
            in->mouse_pos.x = event.motion.x * DEFAULT_W / window->w;
            in->mouse_pos.y = event.motion.y * DEFAULT_H / window->h;
            in->last_input_type = MOUSE;
            break;

        case SDL_MOUSEBUTTONDOWN:
            in->mouse_button[event.button.button] = 1;
            in->last_input_type = MOUSE;
            break;

        case SDL_MOUSEBUTTONUP:
            in->mouse_button[event.button.button] = 0;
            break;

        default:
            break;
        }
    }

    if (!is_in_level && in->last_input_type == MOUSE)
        SDL_ShowCursor(SDL_ENABLE);
    else if (in->last_input_type != MOUSE)
        SDL_ShowCursor(SDL_DISABLE);
}

void handle_quit_event(struct window *window, int is_in_level)
{
    if (window->in->quit)
    {
        free_all(window, is_in_level);
        exit(EXIT_SUCCESS);
    }
}

int handle_escape_event(struct window *window)
{
    if (window->in->key[SDL_SCANCODE_ESCAPE]
        || window->in->c[0].button[SDL_CONTROLLER_BUTTON_B]
        || window->in->c[1].button[SDL_CONTROLLER_BUTTON_B]
        || window->in->mouse_button[SDL_BUTTON_X1]
        || window->in->key[SDL_SCANCODE_AC_BACK]) // Android back button
    {
        window->in->key[SDL_SCANCODE_ESCAPE] = 0;
        window->in->c[0].button[SDL_CONTROLLER_BUTTON_B] = 0;
        window->in->c[1].button[SDL_CONTROLLER_BUTTON_B] = 0;
        window->in->mouse_button[SDL_BUTTON_X1] = 0;
        window->in->key[SDL_SCANCODE_AC_BACK] = 0;

        return 1;
    }

    return 0;
}


int handle_play_event(struct window *window)
{
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

        Mix_PlayChannel(-1, window->sounds->play, 0);

        return 1;
    }

    return 0;
}


void handle_select_arrow_event(struct window *window, unsigned *selected, unsigned max, SDL_Rect areas[])
{
    /* Keyboard and controller */
    // Up
    if (window->in->key[SDL_SCANCODE_UP]
        || window->in->c[0].button[SDL_CONTROLLER_BUTTON_DPAD_UP]
        || window->in->c[1].button[SDL_CONTROLLER_BUTTON_DPAD_UP]
        || (window->in->c[0].axis[SDL_CONTROLLER_AXIS_LEFTY].value <= -MENU_DEAD_ZONE
            && window->in->c[0].axis[SDL_CONTROLLER_AXIS_LEFTY].state)
        || (window->in->c[1].axis[SDL_CONTROLLER_AXIS_LEFTY].value <= -MENU_DEAD_ZONE
            && window->in->c[1].axis[SDL_CONTROLLER_AXIS_LEFTY].state))
    {
        window->in->key[SDL_SCANCODE_UP] = 0;
        window->in->c[0].button[SDL_CONTROLLER_BUTTON_DPAD_UP] = 0;
        window->in->c[1].button[SDL_CONTROLLER_BUTTON_DPAD_UP] = 0;

        if (*selected > 1)
        {
            (*selected)--;
            Mix_PlayChannel(-1, window->sounds->select, 0);
        }
        else if (*selected == 0)
        {
            *selected = 1;
            Mix_PlayChannel(-1, window->sounds->select, 0);
        }
    }

    // Down
    if (window->in->key[SDL_SCANCODE_DOWN]
        || window->in->c[0].button[SDL_CONTROLLER_BUTTON_DPAD_DOWN]
        || window->in->c[1].button[SDL_CONTROLLER_BUTTON_DPAD_DOWN]
        || (window->in->c[0].axis[SDL_CONTROLLER_AXIS_LEFTY].value >= MENU_DEAD_ZONE
            && window->in->c[0].axis[SDL_CONTROLLER_AXIS_LEFTY].state)
        || (window->in->c[1].axis[SDL_CONTROLLER_AXIS_LEFTY].value >= MENU_DEAD_ZONE
            && window->in->c[1].axis[SDL_CONTROLLER_AXIS_LEFTY].state))
    {
        window->in->key[SDL_SCANCODE_DOWN] = 0;
        window->in->c[0].button[SDL_CONTROLLER_BUTTON_DPAD_DOWN] = 0;
        window->in->c[1].button[SDL_CONTROLLER_BUTTON_DPAD_DOWN] = 0;

        if (*selected < max)
        {
            (*selected)++;
            Mix_PlayChannel(-1, window->sounds->select, 0);
        }
    }

    // Mouse and touch screen
    if (window->in->last_input_type == MOUSE)
    {
        unsigned last_selected = *selected;
        *selected = 0;

        for (unsigned i = 0; i < max; i++)
        {
            if (window->in->mouse_pos.x >= areas[i].x
                && window->in->mouse_pos.x < areas[i].x + areas[i].w
                && window->in->mouse_pos.y >= areas[i].y
                && window->in->mouse_pos.y < areas[i].y + areas[i].h)
                *selected = i + 1;
        }

        if (*selected != 0 && last_selected != *selected)
            Mix_PlayChannel(-1, window->sounds->select, 0);
    }
}


int init_controller(struct input *in, unsigned num_controller, Sint32 which)
{
    in->c[num_controller].controller = SDL_GameControllerOpen(which);

    // Error handling
    if (!in->c[num_controller].controller)
        return 0;

    in->c[num_controller].name = (char *) SDL_GameControllerName(in->c[num_controller].controller);
    SDL_Joystick *temp = SDL_GameControllerGetJoystick(in->c[num_controller].controller);

    // Get instance ID
    in->c[num_controller].id = SDL_JoystickInstanceID(temp);

    // Get force feedback
    if (SDL_GameControllerHasRumble(in->c[num_controller].controller))
    {
        in->c[num_controller].has_rumble = 1;
        return 1;
    }

    // If controller hasn't a rumble, it may still be an odd haptic device
    in->c[num_controller].haptic = SDL_HapticOpenFromJoystick(temp);
    if (in->c[num_controller].haptic)
        SDL_HapticRumbleInit(in->c[num_controller].haptic);

    return 1;
}


