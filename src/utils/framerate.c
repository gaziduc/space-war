/*
SDL_framerate.c: framerate manager

Copyright (C) 2001-2012  Andreas Schiffler

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.

Andreas Schiffler -- aschiffler at ferzkopp dot net

---

Edited to work with emscripten

David Ghiassi - 2021
*/
#include <SDL2/SDL2_framerate.h>
#include "framerate.h"
#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#endif


Uint32 getTicks()
{
    Uint32 ticks = SDL_GetTicks();

    if (ticks == 0)
        return 1;

    return ticks;
}


Uint32 framerateDelay(FPSmanager *manager)
{
    Uint32 current_ticks;
    Uint32 target_ticks;
    Uint32 the_delay;
    Uint32 time_passed = 0;

    if (manager == NULL)
        return 0;

    if (manager->baseticks == 0)
        SDL_initFramerate(manager);

    manager->framecount++;
    current_ticks = getTicks();
    time_passed = current_ticks - manager->lastticks;
    manager->lastticks = current_ticks;
    target_ticks = manager->baseticks + (Uint32) ((float) manager->framecount * manager->rateticks);

    if (current_ticks <= target_ticks)
    {
        the_delay = target_ticks - current_ticks;

#ifndef __EMSCREIPTEN__
        SDL_Delay(the_delay);
#else
        emscripten_sleep(the_delay);
#endif
    }
    else
    {
        manager->framecount = 0;
        manager->baseticks = getTicks();
    }

    return time_passed;
}
