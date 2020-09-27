#ifndef INIT_H
#define INIT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL2_framerate.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_net.h>

#define NUM_LEVELS 9
#define NUM_ROTATING_FRAMES 180
#define DEFAULT_W 1920
#define DEFAULT_H 1080
#define MAX_PLAYERS 2
#define NUM_RESOLUTIONS 6

enum object_type
{
    HEALTH = 0,
    SHIELD,
    PLANET,
    GALAXY,
    MISSILE_AROUND,
    NUM_OBJECTS
};

struct collision_texture
{
    SDL_Texture *texture;
    int w;
    int h;
    short *collision;
};

struct textures
{
    struct collision_texture *ship;
    struct collision_texture *shot[3];
    struct collision_texture *enemy;
    SDL_Texture *explosion;
    SDL_Texture *trail;
    struct collision_texture *enemy_shot;
    struct collision_texture *boss;
    struct collision_texture *objects[NUM_OBJECTS];
    struct collision_texture *asteroid;
    struct collision_texture *rotating_enemy[NUM_ROTATING_FRAMES];
    SDL_Texture *aura;
    struct collision_texture *drone;
    struct collision_texture *final_boss;
    struct collision_texture *wall;
};


struct axis
{
    Sint16 value;
    int state;
};

struct controller
{
    SDL_GameController *controller;
    Sint32 id;
    char button[SDL_CONTROLLER_BUTTON_MAX];
    struct axis axis[SDL_CONTROLLER_AXIS_MAX];
    SDL_Haptic *haptic;
};

struct input
{
    char quit;
    char focus_lost;
    char key[SDL_NUM_SCANCODES];
    SDL_Point mouse_pos;
    char mouse_button[5];
    struct controller c;
    char text[8];
};


union texture
{
    struct collision_texture *texture;
    struct collision_texture *textures[NUM_ROTATING_FRAMES];
};


struct list
{
    SDL_Rect pos_src;
    SDL_Rect pos_dst;
    unsigned long framecount;
    SDL_Point speed;
    int health;
    int max_health;
    Uint32 last_time_hurt;
    enum object_type type;
    char enemy_type;
    int rotating;
    int curr_texture;
    union texture texture;
    struct list *next;
};

enum list_type
{
    MY_SHOTS_LIST = 0,
    ENEMY_LIST,
    EXPLOSION_LIST,
    ENEMY_SHOT_LIST,
    BOSS_LIST,
    OBJECT_LIST,
    HUD_LIST,
    NUM_LISTS
};

struct vector
{
    struct path *data;
    size_t size;
    size_t capacity;
    size_t index;
};

struct fonts
{
    TTF_Font *pixel_small_bold;
    TTF_Font *pixel;
    TTF_Font *pixel_large;
    TTF_Font *zero4b_30;
    TTF_Font *zero4b_30_small;
    TTF_Font *zero4b_30_extra_small;
    TTF_Font *calibri;
};

struct sounds
{
    Mix_Chunk *shot;
    Mix_Chunk *explosion;
    Mix_Chunk *power_up;
    Mix_Chunk *select;
    Mix_Chunk *play;
};

enum control
{
    UP = 0,
    LEFT,
    DOWN,
    RIGHT,
    SHOOT,
    BOMB,
    NUM_CONTROLS
};

struct settings
{
    int is_fullscreen;
    int music_volume;
    int sfx_volume;
    int is_force_feedback;
    SDL_Scancode controls[NUM_CONTROLS];
};

struct save
{
    int progress[2][NUM_LEVELS + 1]; // 2 for solo and multi
    int score[2][NUM_LEVELS + 1]; // same goes here
};


enum input_type
{
    KEYBOARD = 0,
    MOUSE,
    CONTROLLER,
    NUM_INPUT_TYPE
};

struct player
{
    SDL_Rect pos;
    int lives;
    int respawn_frame;
    int shield_time;
    int ammo;
    int health;
    int animated_health_low;
    int animated_health_high;
    Uint32 last_shot_time;
    enum input_type input_type;
    int missile_around;
};

struct window
{
    SDL_Window *window;
    int w;
    int h;
    SDL_Renderer *renderer;
    struct textures *img;
    struct input *in;
    FPSmanager *fps;
    struct list *list[NUM_LISTS];
    struct vector *paths;
    Uint32 last_enemy_time;
    struct fonts *fonts;
    int score;
    Mix_Music *music;
    struct sounds *sounds;
    int is_wave_title;
    Uint32 wave_title_time;
    int num_bombs;
    struct universe *universe;
    struct point *stars;
    int bonus;
    struct settings *settings;
    int max_health;
    struct save *save;
    int num_players;
    struct player player[MAX_PLAYERS];
    int is_lan;
    TCPsocket server;
    TCPsocket client;
    int weapon;
    int touched_anim;
    SDL_Point resolutions[NUM_RESOLUTIONS];
    int resolution_index;
    Uint32 mission_start_time;
};

void load_music(struct window *window, const char *filename, int must_free);

struct window *init_all(void);

#endif /* !INIT_H */
