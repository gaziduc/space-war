#include "utils.h"
#include "init.h"
#include "list.h"
#include "path.h"
#include "game.h"
#include "stars.h"
#include "setting.h"
#include "save.h"
#include "event.h"
#include "language.h"
#include "msg_list.h"
#include "menu.h"
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_framerate.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL2_rotozoom.h>


void render_loading_screen(struct window *window)
{
    static Uint32 last_time = 0;
    Uint32 time = SDL_GetTicks();

    static float progress = 0;

    progress += 2.0;

    if (time - last_time >= 17)
    {
        last_time = time;

        SDL_Color white = { 195, 195, 195, 255 };
        SDL_Color blue = { .r = BLUE_R, .g = BLUE_G, .b = BLUE_B, .a = TITLE_ALPHA_MAX };

        char s[128] = { 0 };
        sprintf(s, "%d %%", (int) progress);

        SDL_SetRenderDrawColor(window->renderer, 8, 8, 8, 255);
        SDL_RenderClear(window->renderer);
        render_text(window, window->fonts->zero4b_30, "SPACE WAR", blue, POS_CENTERED, 200);

        SDL_Rect pos = { .x = DEFAULT_W / 2 - 300, .y = 665, .w = (int) (progress * 6), .h = 45 };
        resize_pos_for_resolution(window, &pos);
        SDL_SetRenderDrawColor(window->renderer, 90, 230, 29, 192);
        SDL_RenderFillRect(window->renderer, &pos);

        SDL_Rect pos_bar = { .x = DEFAULT_W / 2 - 300, .y = 665, .w = 600, .h = 45 };
        resize_pos_for_resolution(window, &pos_bar);
        SDL_SetRenderDrawColor(window->renderer, 225, 225, 225, 255);
        SDL_RenderDrawRect(window->renderer, &pos_bar);

        render_text(window, window->fonts->craft_large, s, white, POS_CENTERED, 673);

        SDL_RenderPresent(window->renderer);
    }
}


static void add_resolution(struct window *window, int index, int w, int h)
{
    window->resolutions[index].x = w;
    window->resolutions[index].y = h;
}

static struct window *init_window(void)
{
    struct window *window = xcalloc(1, sizeof(struct window), NULL, NULL);

    SDL_DisplayMode dm;
    if (SDL_GetDesktopDisplayMode(0, &dm) != 0)
        error("SDL_GetDesktopDisplayMode failed", SDL_GetError(), window->window, window->renderer);

    add_resolution(window, 0, dm.w, dm.h);
    add_resolution(window, 1, 1280, 720);
    add_resolution(window, 2, 1366, 768);
    add_resolution(window, 3, 1600, 900);
    add_resolution(window, 4, DEFAULT_W, DEFAULT_H);
    add_resolution(window, 5, 2560, 1440);
    add_resolution(window, 6, 3840, 2160);

    load_settings(window);

    window->window = SDL_CreateWindow("Space War",
                                      SDL_WINDOWPOS_CENTERED_DISPLAY(window->settings->display_num),
                                      SDL_WINDOWPOS_CENTERED_DISPLAY(window->settings->display_num),
                                      window->w,
                                      window->h,
                                      0);

    if (!window->window)
        error("Could not create window", SDL_GetError(), NULL, NULL);

    window->renderer = SDL_CreateRenderer(window->window, -1, SDL_RENDERER_PRESENTVSYNC);

    if (!window->renderer)
        error("Could not create renderer", SDL_GetError(), window->window, NULL);


    if (window->settings->display_num >= SDL_GetNumVideoDisplays())
    {
        window->settings->display_num = 0;
        write_settings(window);
    }

    // To fix a bug on linux...
    SDL_SetWindowPosition(window->window,
                          SDL_WINDOWPOS_CENTERED_DISPLAY(window->settings->display_num),
                          SDL_WINDOWPOS_CENTERED_DISPLAY(window->settings->display_num));

    // Put it here so we have already chosen the correct display monitor
    if (window->settings->is_fullscreen)
        SDL_SetWindowFullscreen(window->window, SDL_WINDOW_FULLSCREEN_DESKTOP);

    return window;
}


static void load_textures(struct window *window)
{
    window->img = xmalloc(sizeof(struct textures), window->window, window->renderer);

    char s[50] = { 0 };

    for (unsigned i = 0; i < NUM_ANIM_SHIP; i++)
    {
        sprintf(s, "data/ship%u.png", i);
        window->img->ship[i] = load_texture_collision(s, window);
    }

    window->img->shot[0] = load_texture_collision("data/shot0.png", window);
    window->img->shot[1] = load_texture_collision("data/shot1.png", window);
    window->img->shot[2] = load_texture_collision("data/shot2.png", window);
    window->img->enemy = load_texture_collision("data/enemy.png", window);
    window->img->explosion[0] = load_texture("data/explosion0.png", window);
    window->img->explosion[1] = load_texture("data/explosion1.png", window);
    window->img->trail = load_texture("data/trail.png", window);
    window->img->enemy_shot = load_texture_collision("data/enemy_shot.png", window);
    window->img->enemy_shot_teleguided = load_texture_collision("data/enemy_shot_teleguided.png", window);
    window->img->enemy_shot_explode = load_texture_collision("data/enemy_shot_explode.png", window);

    window->img->bosses[0] = load_texture_collision("data/boss0.png", window);
    window->img->bosses[1] = load_texture_collision("data/boss1.png", window);
    window->img->bosses[2] = load_texture_collision("data/boss2.png", window);
    window->img->bosses[9] = load_texture_collision("data/boss9.png", window);

    window->img->objects[HEALTH] = load_texture_collision("data/health.png", window);
    window->img->objects[SHIELD] = load_texture_collision("data/shield.png", window);
    window->img->objects[PLANET] = load_texture_collision("data/planet.png", window);
    window->img->objects[GALAXY] = load_texture_collision("data/galaxy.png", window);
    window->img->objects[MISSILE_AROUND] = load_texture_collision("data/missile.png", window);
    window->img->aura = load_texture("data/aura.png", window);
    window->img->asteroid = load_texture_collision("data/asteroid.png", window);

    // Load/create rotating enemy animation
    SDL_Surface *original = IMG_Load("data/rotating_enemy.png");
    if (!original)
        error("Could not load surface", IMG_GetError(), window->window, window->renderer);

    window->img->rotating_enemy[0] = get_texture_collision(original, window);

    for (size_t i = 1; i < NUM_ROTATING_FRAMES; i++)
    {
        SDL_Surface *temp = rotozoomSurface(original, i * 2.0, 1.0, SMOOTHING_ON);
        window->img->rotating_enemy[i] = get_texture_collision(temp, window);

        SDL_FreeSurface(temp);
    }

    SDL_FreeSurface(original);

    window->img->drone = load_texture_collision("data/drone.png", window);
    window->img->wall = load_texture_collision("data/wall.png", window);
    window->img->a_button = load_texture("data/a_button.png", window);
    window->img->b_button = load_texture("data/b_button.png", window);
    window->img->trophy = load_texture("data/trophy_gold.png", window);
    window->img->controller = load_texture("data/controller.png", window);
    window->img->bomb = load_texture("data/bomb.png", window);
    window->img->padlock = load_texture("data/padlock.png", window);
    window->img->play_icon = load_texture("data/play.png", window);
    window->img->wheel = load_texture("data/wheel.png", window);
    window->img->help = load_texture("data/help.png", window);
    window->img->copyright = load_texture("data/copyright.png", window);
    window->img->exit_logo = load_texture("data/exit.png", window);
}


static void load_fonts(struct window *window)
{
    window->fonts = xmalloc(sizeof(struct fonts), window->window, window->renderer);

    window->fonts->craft_small = load_font(window, "data/minecraft.ttf", 25);
    window->fonts->craft = load_font(window, "data/minecraft.ttf", 30);
    window->fonts->craft_large = load_font(window, "data/minecraft.ttf", 37);

    window->fonts->zero4b_30 = load_font(window, "data/04b_30.ttf", 120);
    window->fonts->zero4b_30_small = load_font(window, "data/04b_30.ttf", 60);
    window->fonts->zero4b_30_extra_small = load_font(window, "data/04b_30.ttf", 40);
}

void load_music_and_play(struct window *window, const char *filename, int must_free)
{
    if (must_free)
        Mix_FreeMusic(window->music);

    window->music = Mix_LoadMUS(filename);
    if (!window->music)
        error("Could not load music", Mix_GetError(), window->window, window->renderer);

    if (Mix_PlayMusic(window->music, -1) == -1)
        error("Could not play music", Mix_GetError(), window->window, window->renderer);
}

static void load_sounds(struct window *window)
{
    window->sounds = xmalloc(sizeof(struct sounds), window->window, window->renderer);

    window->sounds->shot = load_sound(window, "data/shot.wav");
    window->sounds->explosion = load_sound(window, "data/explosion.wav");
    window->sounds->power_up = load_sound(window, "data/power_up.wav");
    window->sounds->select = load_sound(window, "data/select.wav");
    window->sounds->play = load_sound(window, "data/play.wav");
    window->sounds->trophy = load_sound(window, "data/trophy.wav");
}


struct window *init_all(void)
{
    // Init SDL2
#ifndef __EMSCRIPTEN__
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC) != 0)
#else
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0)
#endif
        error("Could not load SDL2", SDL_GetError(), NULL, NULL);

    // Create window and renderer
    struct window *window = init_window();

    // Enable blending (alpha)
    SDL_SetRenderDrawBlendMode(window->renderer, SDL_BLENDMODE_BLEND);

    // Window icon
    SDL_Surface *icon = SDL_LoadBMP("data/icon.bmp");
    if (!icon)
        error("Could not load surface", SDL_GetError(), window->window, window->renderer);
    SDL_SetWindowIcon(window->window, icon);
    SDL_FreeSurface(icon);

    // Init SDL2_image
    int img_flags = IMG_INIT_PNG;
    int img_initted = IMG_Init(img_flags);
    if ((img_initted & img_flags) != img_flags)
        error("Could not load SDL2_image", IMG_GetError(), window->window, window->renderer);

    // Init SDL2_tff
    if (TTF_Init() == -1)
        error("Could not load SDL2_ttf", TTF_GetError(), window->window, window->renderer);

    load_fonts(window);
    load_language_file(window, "data/en.txt");

    // Load textures
    load_textures(window);


    // Init inputs
    window->in = xcalloc(1, sizeof(struct input), window->window, window->renderer);

    // Init framerate manager
    window->fps = xmalloc(sizeof(FPSmanager), window->window, window->renderer);
    SDL_initFramerate(window->fps);
    if (SDL_setFramerate(window->fps, 60) == -1)
        error("Could not set framerate", "Could not set framerate to 60 Hz", window->window, window->renderer);

    // Init linked lists for shots, enemies, ...
    for (enum list_type i = 0; i < NUM_LISTS; i++)
        init_list(window, i);

    // We can now shot
    for (unsigned i = 0; i < window->num_players; i++)
        window->player[i].last_shot_time = 0;

    window->paths = NULL;

#ifndef __EMSCRIPTEN__
    // Init SDL2_mixer
    int mix_flags = MIX_INIT_OGG;
    int mix_initted = Mix_Init(mix_flags);
    if ((mix_initted & mix_flags) != mix_flags)
        error("Could not load SDL2_mixer", Mix_GetError(), window->window, window->renderer);
#endif

    // Load music with volume settings
    Mix_AllocateChannels(32);

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
        error("Could not initialize SDL2_mixer", Mix_GetError(), window->window, window->renderer);

    Mix_VolumeMusic(window->settings->music_volume);
    Mix_Volume(-1, window->settings->sfx_volume);

    load_sounds(window);

    // Init SDL2_net
    if (SDLNet_Init() == -1)
        error("Could not initialize SDL2_net", SDLNet_GetError(), window->window, window->renderer);

    // Initialize the stars lib
    new_universe(&window->universe, 256, window);

    // Load save file and progress
    read_save(window);

    // LAN meesages
    window->msg_list = xmalloc(sizeof(struct msg_list), window->window, window->renderer);
    window->msg_list->next = NULL;

    // Trophies
    window->trophy.id[0] = -1;

    return window;
}
