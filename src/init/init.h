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
#define NUM_RESOLUTIONS 7
#define MAX_NUM_FINGERS 2

/* Colors */
#define BLUE_R 0
#define BLUE_G 162
#define BLUE_B 232

#define GREEN_R 153
#define GREEN_G 217
#define GREEN_B 234

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
    char *collision;
};

enum trophy_value
{
    BRONZE = 0,
    SILVER,
    GOLD,
    NUM_TROPHY_VALUES,
};

enum trophies
{
    REALLY_HARD_SUCCESS,
    ARCADE_SUCCESS,
    COMBO_LOVER,
    BOMB_SAVER,
    PERFECT_BOMB,
    SPEEDRUN,
    UNITED_WE_STAND,
    IS_IT_POSSIBLE,
    AMMO_COLLECTOR,
    OVER_THE_WORLD,
    NUM_TROPHIES
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
    SDL_Texture *a_button;
    SDL_Texture *b_button;
    SDL_Texture *trophy;
    SDL_Texture *controller;
    SDL_Texture *bomb;
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


enum input_type
{
    KEYBOARD = 0,
    MOUSE,
    CONTROLLER,
    NUM_INPUT_TYPE
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
    char finger[MAX_NUM_FINGERS];
    enum input_type last_input_type;

};


union texture
{
    struct collision_texture *texture;
    struct collision_texture *textures[NUM_ROTATING_FRAMES];
};


struct list
{
    SDL_Rect pos_src;
    SDL_FRect pos_dst;
    unsigned long framecount;
    SDL_FPoint speed;
    int health;
    int max_health;
    Uint32 last_time_hurt;
    Uint32 first_time_hurt;
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
    TTF_Font *pixel;
    TTF_Font *pixel_large;
    TTF_Font *craft_small;
    TTF_Font *craft;
    TTF_Font *craft_large;
    TTF_Font *zero4b_30;
    TTF_Font *zero4b_30_small;
    TTF_Font *zero4b_30_extra_small;
};

struct sounds
{
    Mix_Chunk *shot;
    Mix_Chunk *explosion;
    Mix_Chunk *power_up;
    Mix_Chunk *select;
    Mix_Chunk *play;
    Mix_Chunk *trophy;
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
    int mouse_sensitivity;
    int show_tutorial;
};




struct save
{
    int progress[2][NUM_LEVELS + 1]; // 2 for solo and multi
    int score[2][NUM_LEVELS + 1]; // same goes here
    int trophies[NUM_TROPHIES];
};


struct player
{
    SDL_FRect pos;
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


struct state
{
    Uint16 pos_x;
    Uint16 pos_y;
    Uint16 health;
    Uint16 ammo;
    char is_shooting;
    char throw_bomb;
    char has_shield;
    char state; // 0 = is_in_menu, 1 = is_in_level, 2 = quit, 3 = end screen

};


enum msg_type
{
    ACCEPT_MSG,
    RESTART_MSG,
    MENU_MSG,
    LEVEL_MSG,
    POSITION_MSG,
    SHOOT_MSG,
    BOMB_MSG,
    GET_TIME_MSG,
    TIME_MSG,
    QUIT_MSG,
    Z_MSG
};

struct level
{
    char level_num;
    char level_difficulty;
    char weapon;
    Uint32 start_mission_ticks;
};

union msg_content
{
    struct level lvl;
    SDL_Point point;
    Uint32 ticks;
    char boolean;
};


struct msg
{
    enum msg_type type;
    union msg_content content;
};


enum texts
{
    PLAY = 0,
    SETTINGS,
    HELP,
    TROPHIES,
    CREDITS,
    QUIT,
    SELECT_MODE,
    ONE_PLAYER,
    TWO_PLAYERS_LOCAL,
    TWO_PLAYERS_NETWORK,
    BACK_1,
    SELECT_MISSION,
    MISSION_D_D_S,
    ARCADE_MODE_S,
    BACK_2,
    THE_MILKY_WAY,
    ANDROMEDA_GALAXY,
    HYPERSPACE,
    NEW_UNIVERSE,
    SPATIAL_ARMY,
    BOSSES,
    NEW_PLANET,
    SPACE_TUNNEL,
    THE_END,
    ARCADE_MODE,
    MISSION_D_D___S,
    EASY__,
    HARD__,
    REALLY_HARD__,
    BACK_3,
    EASY_CONDITIONS,
    HARD_CONDITIONS,
    REALLY_HARD_CONDITIONS,
    CHOOSE_WEAPON,
    WEAPON_CONDITIONS,
    BACK_4,
    SUMMARY,
    MODE_S,
    WEAPON,
    START_MISSION,
    BACK_5,
    CREATE_OR_JOIN,
    CREATE,
    JOIN,
    BACK_6,
    USER_AT,
    ACCEPT,
    DECLINE,
    WAITING_FOR_SOMEONE,
    YOUR_IP_LOCAL,
    YOUR_IP_ONLINE,
    SEARCHING_FOR_ONLINE_IP,
    TO_PLAY_ONLINE_1,
    TO_PLAY_ONLINE_2,
    TO_PLAY_ONLINE_3,
    CONNECTING,
    WAITING_FOR_OTHER,
    INVALID_IP,
    PAUSE,
    RESUME,
    SETTINGS_2,
    HELP_2,
    ESCAPE,
    AUDIO_TXT,
    MUSIC_VOLUME_S,
    SFX_VOLUME_S,
    VIDEO_TXT,
    FULLSCREEN_S,
    RESOLUTION_DXD_S,
    INPUTS_TXT,
    P1_INPUT_S,
    P2_INPUT_S,
    KEYBOARD_CONTROLS,
    MOUSE_SENSITIVITY_S,
    CONTROLLER_FORCE_FEEDBACK_S,
    BACK_7,
    UP_TXT,
    LEFT_TXT,
    DOWN_TXT,
    RIGHT_TXT,
    SHOOT_TXT,
    BOMB_TXT,
    RESET_INPUTS,
    BACK_8,
    SCORE_D,
    BOMBS_D,
    AMMO_D,
    AMMO_999,
    NETWORK,
    KEYBOARD_TXT,
    MOUSE_TXT,
    CONTROLLER_TXT,
    ENTER_KEY,
    CURRENT_KEY,
    CONTINUE,
    SUCCESS,
    SCORE_D_2,
    HEALTH_BONUS_D,
    BOMBS_BONUS_D,
    DIFFICULTY_BONUS_D,
    TOTAL_D,
    NEW_BEST,
    WAITING_FOR_THE_SERVER,
    FAILURE,
    RETRY,
    BACK_9,
    BEST_D,
    YES,
    NO,
    NATIVE,
    LOW_X1,
    HIGH_X2,
    MADE_BY_DAVID,
    COMPATIBLE_WITH_CONTROLLERS,
    PRESS_TO_GET_IP,
    SYNC_CLIENT_SERVER,
    SELECT,
    BACK_LOWERCASE,
    CHOOSE_DIFFICULTY,
    ESCAPE_TO_CANCEL,
    TROPHY_1_TITLE,
    TROPHY_1_LINE_1,
    TROPHY_1_LINE_2,
    TROPHY_2_TITLE,
    TROPHY_2_LINE_1,
    TROPHY_2_LINE_2,
    TROPHY_3_TITLE,
    TROPHY_3_LINE_1,
    TROPHY_3_LINE_2,
    TROPHY_4_TITLE,
    TROPHY_4_LINE_1,
    TROPHY_4_LINE_2,
    TROPHY_5_TITLE,
    TROPHY_5_LINE_1,
    TROPHY_5_LINE_2,
    TROPHY_6_TITLE,
    TROPHY_6_LINE_1,
    TROPHY_6_LINE_2,
    TROPHY_7_TITLE,
    TROPHY_7_LINE_1,
    TROPHY_7_LINE_2,
    TROPHY_8_TITLE,
    TROPHY_8_LINE_1,
    TROPHY_8_LINE_2,
    TROPHY_9_TITLE,
    TROPHY_9_LINE_1,
    TROPHY_9_LINE_2,
    TROPHY_10_TITLE,
    TROPHY_10_LINE_1,
    TROPHY_10_LINE_2,
    NUM_TXT
};

struct trophy
{
    int is_unlocking_trophies;
    int is_unlocking_fade_in;
    int is_unlocking_idleing;
    int is_unlocking_fade_out;
    int offset_x;
    int trophy_unlocked;
    int id[NUM_TROPHIES];
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
    int initial_bombs;
    struct universe *universe;
    struct point *stars;
    int bonus;
    struct settings *settings;
    int max_health;
    struct save *save;
    unsigned num_players;
    struct player player[MAX_PLAYERS];
    int is_lan;
    TCPsocket server;
    TCPsocket client;
    unsigned weapon;
    int touched_anim;
    SDL_Point resolutions[NUM_RESOLUTIONS];
    int resolution_index;
    Uint32 mission_start_time;
    char *txt[NUM_TXT];
    struct msg_list *msg_list;
    int accepted;
    int restart;
    SDL_Point shake;
    Uint32 client_request_time;
    Uint32 ticks;
    Uint32 client_time;
    Uint32 last_sync_time;
    unsigned combo;
    Uint32 combo_time;
    unsigned last_combo;
    Uint32 last_combo_time;
    struct trophy trophy;
    int combo_lover;
    int num_enemies_collided;
};

void render_loading_screen(struct window *window);

void load_music(struct window *window, const char *filename, int must_free);

struct window *init_all(void);

#endif /* !INIT_H */
