#include "event.h"
#include "init.h"
#include "free.h"
#include <SDL2/SDL.h>

void update_events(struct input *in, struct window *window)
{
    in->quit = 0;
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

        // Keyboard events
        case SDL_KEYDOWN:
            in->key[event.key.keysym.scancode] = 1;
            break;
        case SDL_KEYUP:
            in->key[event.key.keysym.scancode] = 0;
            break;

        // Controller events
        case SDL_CONTROLLERDEVICEADDED:
            if (!in->c.controller)
            {
                if (SDL_IsGameController(event.cdevice.which))
                {
                    in->c.controller = SDL_GameControllerOpen(event.cdevice.which);
                    window->in->c.id = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(window->in->c.controller));
                }
                else
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Controller not reconized",
                                             "Your controller is not compatible.", window->window);
            }
            break;

        case SDL_CONTROLLERDEVICEREMOVED:
            if (event.cdevice.which == in->c.id)
            {
                SDL_GameControllerClose(in->c.controller);
                in->c.controller = NULL;
            }
            break;

        case SDL_CONTROLLERBUTTONDOWN:
            in->c.button[event.cbutton.button] = 1;
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
}

int handle_escape_event(struct window *window)
{
    if (window->in->key[SDL_SCANCODE_ESCAPE]
        || window->in->c.button[SDL_CONTROLLER_BUTTON_BACK])
    {
        window->in->key[SDL_SCANCODE_ESCAPE] = 0;
        window->in->c.button[SDL_CONTROLLER_BUTTON_BACK] = 0;

        return 1;
    }

    return 0;
}


int handle_play_event(struct window *window)
{
    if (window->in->key[SDL_SCANCODE_RETURN]
        || window->in->key[SDL_SCANCODE_KP_ENTER]
        || window->in->c.button[SDL_CONTROLLER_BUTTON_A])
    {
        window->in->key[SDL_SCANCODE_RETURN] = 0;
        window->in->key[SDL_SCANCODE_KP_ENTER] = 0;
        window->in->c.button[SDL_CONTROLLER_BUTTON_A] = 0;

        Mix_PlayChannel(-1, window->sounds->play, 0);

        return 1;
    }

    return 0;
}


void handle_select_arrow_event(struct window *window, int *selected, int max)
{
    if (window->in->key[SDL_SCANCODE_UP]
        || window->in->c.button[SDL_CONTROLLER_BUTTON_DPAD_UP]
        || (window->in->c.axis[SDL_CONTROLLER_AXIS_LEFTY].value <= -DEAD_ZONE
            && window->in->c.axis[SDL_CONTROLLER_AXIS_LEFTY].state))
    {
        window->in->key[SDL_SCANCODE_UP] = 0;
        window->in->c.button[SDL_CONTROLLER_BUTTON_DPAD_UP] = 0;

        if (*selected > 1)
        {
            (*selected)--;
            Mix_PlayChannel(-1, window->sounds->select, 0);
        }
    }

    if (window->in->key[SDL_SCANCODE_DOWN]
        || window->in->c.button[SDL_CONTROLLER_BUTTON_DPAD_DOWN]
        || (window->in->c.axis[SDL_CONTROLLER_AXIS_LEFTY].value >= DEAD_ZONE
            && window->in->c.axis[SDL_CONTROLLER_AXIS_LEFTY].state))
    {
        window->in->key[SDL_SCANCODE_DOWN] = 0;
        window->in->c.button[SDL_CONTROLLER_BUTTON_DPAD_DOWN] = 0;

        if (*selected < max)
        {
            (*selected)++;
            Mix_PlayChannel(-1, window->sounds->select, 0);
        }
    }
}
