#include "event.h"
#include "init.h"
#include "free.h"
#include "pause.h"
#include <stdlib.h>
#include <SDL2/SDL.h>

void update_events(struct input *in, struct window *window)
{
    in->quit = 0;
    memset(in->text, '\0', sizeof(in->text));
    in->wheel.x = 0;
    in->wheel.y = 0;
    SDL_Event event;

    // Reset axis state
    for (int i = 0; i < SDL_CONTROLLER_AXIS_MAX; i++)
        in->c.axis[i].state = 0;

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
            if (!in->c.controller)
            {
                if (SDL_IsGameController(event.cdevice.which))
                    init_controller(in, event.cdevice.which);
            }
            break;

        case SDL_CONTROLLERDEVICEREMOVED:
            if (event.cdevice.which == in->c.id)
            {
                // Free haptic
                if (in->c.haptic)
                {
                    SDL_HapticClose(in->c.haptic);
                    in->c.haptic = NULL;
                }

                SDL_GameControllerClose(in->c.controller);
                in->c.controller = NULL;
            }
            break;

        case SDL_CONTROLLERBUTTONDOWN:
            in->c.button[event.cbutton.button] = 1;
            in->last_input_type = CONTROLLER;
            break;

        case SDL_CONTROLLERBUTTONUP:
            in->c.button[event.cbutton.button] = 0;
            break;

        case SDL_CONTROLLERAXISMOTION:
            // For menus
            if (abs(event.caxis.value) >= DEAD_ZONE
                && abs(in->c.axis[event.caxis.axis].value) < DEAD_ZONE)
                in->c.axis[event.caxis.axis].state = 1;

            in->c.axis[event.caxis.axis].value = event.caxis.value;
            in->last_input_type = CONTROLLER;
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

        case SDL_MOUSEWHEEL:
            in->wheel.x = event.wheel.x;
            in->wheel.y = event.wheel.y;
            in->last_input_type = KEYBOARD;
            break;

        /* Touch events */
        case SDL_FINGERDOWN:
            if (event.tfinger.fingerId < MAX_NUM_FINGERS)
            {
                in->finger[event.tfinger.fingerId] = 1;
                in->touch_pos[event.tfinger.fingerId].x = event.tfinger.x * window->w;
                in->touch_pos[event.tfinger.fingerId].y = event.tfinger.y * window->h;
                in->last_input_type = TOUCH;
            }
            break;

        case SDL_FINGERMOTION:
            if (event.tfinger.fingerId < MAX_NUM_FINGERS)
            {
                in->touch_pos[event.tfinger.fingerId].x = event.tfinger.x * window->w;
                in->touch_pos[event.tfinger.fingerId].y = event.tfinger.y * window->h;
                in->last_input_type = TOUCH;
            }
            break;

        case SDL_FINGERUP:
            if (event.tfinger.fingerId < MAX_NUM_FINGERS)
            {
                in->finger[event.tfinger.fingerId] = 0;
                in->touch_pos[event.tfinger.fingerId].x = event.tfinger.x * window->w;
                in->touch_pos[event.tfinger.fingerId].y = event.tfinger.y * window->h;
            }
            break;

        default:
            break;
        }
    }
}

void handle_quit_event(struct window *window, int is_in_level)
{
    if (window->in->quit)
    {
        free_all(window, is_in_level);
        exit(EXIT_SUCCESS);
    }

    /* If is in level, than you have to call handle_focus_lost_event explictly */
    if (!is_in_level)
        handle_focus_lost_event(window);
}

int handle_escape_event(struct window *window)
{
    if (window->in->key[SDL_SCANCODE_ESCAPE]
        || window->in->c.button[SDL_CONTROLLER_BUTTON_BACK]
        || window->in->c.button[SDL_CONTROLLER_BUTTON_START]
        || window->in->mouse_button[SDL_BUTTON_X1]
        || window->in->key[SDL_SCANCODE_AC_BACK]) // Android back button
    {
        window->in->key[SDL_SCANCODE_ESCAPE] = 0;
        window->in->c.button[SDL_CONTROLLER_BUTTON_BACK] = 0;
        window->in->c.button[SDL_CONTROLLER_BUTTON_START] = 0;
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
        || window->in->c.button[SDL_CONTROLLER_BUTTON_A]
        || window->in->mouse_button[SDL_BUTTON_LEFT])
    {
        window->in->key[SDL_SCANCODE_RETURN] = 0;
        window->in->key[SDL_SCANCODE_KP_ENTER] = 0;
        window->in->c.button[SDL_CONTROLLER_BUTTON_A] = 0;
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
        || window->in->c.button[SDL_CONTROLLER_BUTTON_DPAD_UP]
        || (window->in->c.axis[SDL_CONTROLLER_AXIS_LEFTY].value <= -DEAD_ZONE
            && window->in->c.axis[SDL_CONTROLLER_AXIS_LEFTY].state)
        || window->in->wheel.y > 0)
    {
        window->in->key[SDL_SCANCODE_UP] = 0;
        window->in->c.button[SDL_CONTROLLER_BUTTON_DPAD_UP] = 0;

        if (*selected > 1)
        {
            (*selected)--;
            Mix_PlayChannel(-1, window->sounds->select, 0);
        }
    }

    // Down
    if (window->in->key[SDL_SCANCODE_DOWN]
        || window->in->c.button[SDL_CONTROLLER_BUTTON_DPAD_DOWN]
        || (window->in->c.axis[SDL_CONTROLLER_AXIS_LEFTY].value >= DEAD_ZONE
            && window->in->c.axis[SDL_CONTROLLER_AXIS_LEFTY].state)
        || window->in->wheel.y < 0)
    {
        window->in->key[SDL_SCANCODE_DOWN] = 0;
        window->in->c.button[SDL_CONTROLLER_BUTTON_DPAD_DOWN] = 0;

        if (*selected < max)
        {
            (*selected)++;
            Mix_PlayChannel(-1, window->sounds->select, 0);
        }
    }

    // Mouse and touch screen
    if (window->in->last_input_type == MOUSE)
    {
        *selected = 0;

        for (unsigned i = 0; i < max; i++)
        {
            if (window->in->mouse_pos.x >= areas[i].x
                && window->in->mouse_pos.x < areas[i].x + areas[i].w
                && window->in->mouse_pos.y >= areas[i].y
                && window->in->mouse_pos.y < areas[i].y + areas[i].h)
                *selected = i + 1;
        }
    }
    else if (window->in->last_input_type == TOUCH)
    {
        *selected = 0;

        for (unsigned i = 0; i < max; i++)
        {
            if (window->in->touch_pos[0].x >= areas[i].x
                && window->in->touch_pos[0].x < areas[i].x + areas[i].w
                && window->in->touch_pos[0].y >= areas[i].y
                && window->in->touch_pos[0].y < areas[i].y + areas[i].h)
                *selected = i + 1;
        }
    }
}


void init_controller(struct input *in, Sint32 which)
{
    in->c.controller = SDL_GameControllerOpen(which);
    SDL_Joystick *temp = SDL_GameControllerGetJoystick(in->c.controller);

    // Get instance ID
    in->c.id = SDL_JoystickInstanceID(temp);

    // Get force feedback
    in->c.haptic = SDL_HapticOpenFromJoystick(temp);
    if (in->c.haptic)
        SDL_HapticRumbleInit(in->c.haptic);
}


int handle_focus_lost_event(struct window *window)
{
    if (!window->in->focus_lost)
        return 0;

    Uint32 begin = SDL_GetTicks();

    while (window->in->focus_lost)
    {
        update_events(window->in, window);
        handle_quit_event(window, 1); // 1 because else handle_quit_event calls
                                      // handle_focus_lost_event
        SDL_framerateDelay(window->fps);
    }

    delay_times(window, begin);

    return 1;
}

