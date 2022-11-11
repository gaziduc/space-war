#include "init.h"
#include "menu.h"
#include "event.h"
#include "utils.h"
#include "net.h"
#include "level.h"
#include "game.h"
#include "lobby.h"
#include "ip.h"
#include "msg_list.h"
#include "weapon.h"
#include "background.h"
#include "vector.h"
#include "ready.h"
#include "string_vec.h"
#include "enemy.h"
#include "shot.h"
#include "object.h"
#include "explosion.h"
#include "hud.h"
#include "boss.h"
#include <stdio.h>
#include <stdlib.h> // for atoi
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_net.h>


char buf[40]; // 40 = Max IPv6 len + 1
int is_connecting;
int is_connected;
char err[256];
int accepting;
int has_online_ip;
const char *online_ip;


static void render_create_or_join_texts(struct window *window, Uint32 begin,
                                        int selected_item)
{
    Uint32 alpha = SDL_GetTicks() - begin;

    if (alpha > TITLE_ALPHA_MAX)
        alpha = TITLE_ALPHA_MAX;
    else if (alpha == 0)
        alpha = 1;

    SDL_Color blue = { .r = BLUE_R, .g = BLUE_G, .b = BLUE_B, .a = alpha };
    SDL_Color green = { .r = GREEN_R, .g = GREEN_G, .b = GREEN_B, .a = alpha };
    SDL_Color orange = { 255, 127, 39, alpha };

    // Render title
    render_text(window, window->fonts->zero4b_30_small, window->txt[CREATE_OR_JOIN],
                orange, 150, 150);

    // Render items
    char *s_list[3] = { window->txt[CREATE], window->txt[JOIN], window->txt[BACK_6] };

    for (int i = 1; i <= 3; i++)
    {
        if (selected_item != i)
            render_text(window, window->fonts->zero4b_30_small, s_list[i - 1], blue,
                        150, 670 + (i - 1) * 100);
        else
            render_text(window, window->fonts->zero4b_30_small, s_list[i - 1], green,
                        150, 670 + (i - 1) * 100);
    }
}


void create_or_join(struct window *window)
{
    int escape = 0;
    unsigned selected_item = 1;
    Uint32 begin = SDL_GetTicks();
    SDL_Rect areas[3];

    for (unsigned i = 0; i < 3; i++)
    {
        areas[i].x = 150;
        areas[i].y = 670 + i * 100;
        TTF_SizeText(window->fonts->zero4b_30_small, window->txt[CREATE + i], &areas[i].w, &areas[i].h);
    }

    while (!escape)
    {
        // Get and handle events
        update_events(window->in, window, 0);
        handle_quit_event(window, 0);
        handle_select_arrow_event(window, &selected_item, 3, areas);
        escape = handle_escape_event(window);

        if (selected_item > 0 && handle_play_event(window))
        {
            switch (selected_item)
            {
                case 1: // Create
                    create_server(window);
                    break;

                case 2: // Join
                    connect_to_server(window);
                    break;

                case 3:
                    escape = 1;
                    break;

                default:
                    break;
            }

            begin = SDL_GetTicks();
        }

        // Display black background
        SDL_SetRenderDrawColor(window->renderer, 8, 8, 8, 255);
        SDL_RenderClear(window->renderer);

        // Process/Draw all the things
        render_stars(window);
        render_create_or_join_texts(window, begin, selected_item);
        render_controller_input_texts(window, begin, 1);
        SDL_RenderPresent(window->renderer);

        // Wait a frame
        frame_delay(window->fps);
    }
}


static void render_accept_client_texts(struct window *window, Uint32 begin,
                                       int selected_item, char *ip_str)
{
    Uint32 alpha = SDL_GetTicks() - begin;

    if (alpha > TITLE_ALPHA_MAX)
        alpha = TITLE_ALPHA_MAX;
    else if (alpha == 0)
        alpha = 1;

    SDL_Color blue = { .r = BLUE_R, .g = BLUE_G, .b = BLUE_B, .a = alpha };
    SDL_Color green = { .r = GREEN_R, .g = GREEN_G, .b = GREEN_B, .a = alpha };
    SDL_Color orange = { 255, 127, 39, alpha };

    char text[128] = { 0 };
    sprintf(text, window->txt[USER_AT], ip_str);

    // Render title
    render_text(window, window->fonts->zero4b_30_extra_small, text, orange, 150, 150);

    // Render items
    char *s_list[2] = { window->txt[ACCEPT], window->txt[DECLINE] };

    for (int i = 1; i <= 2; i++)
    {
        if (selected_item != i)
            render_text(window, window->fonts->zero4b_30_small, s_list[i - 1], blue,
                        150, 770 + (i - 1) * 100);
        else
            render_text(window, window->fonts->zero4b_30_small, s_list[i - 1], green,
                        150, 770 + (i - 1) * 100);
    }
}




static void accept_client(struct window *window, char *ip_str)
{
    int escape = 0;
    unsigned selected_item = 1;
    Uint32 begin = SDL_GetTicks();
    SDL_Rect areas[2];

    for (unsigned i = 0; i < 2; i++)
    {
        areas[i].x = 150;
        areas[i].y = 770 + i * 100;
        TTF_SizeText(window->fonts->zero4b_30_small, window->txt[ACCEPT + i], &areas[i].w, &areas[i].h);
    }

    while (!escape)
    {
        // Get and handle events
        update_events(window->in, window, 0);
        handle_quit_event(window, 0);
        handle_select_arrow_event(window, &selected_item, 2, areas);

        if (selected_item > 0 && handle_play_event(window))
        {
            switch (selected_item)
            {
                case 1: // Accept
                    SDL_CreateThread(recv_thread, "recv_thread", window);

                    struct msg accept_msg = { .type = ACCEPT_MSG };
                    accept_msg.content.boolean = 1;
                    send_msg(window, &accept_msg);

                    while (handle_messages(window, "G"))
                    {
                        Uint32 alpha = SDL_GetTicks() - begin;

                        if (alpha > TITLE_ALPHA_MAX)
                            alpha = TITLE_ALPHA_MAX;
                        else if (alpha == 0)
                            alpha = 1;

                        SDL_Color white = { 255, 255, 255, alpha };

                        // Render Background
                        SDL_SetRenderDrawColor(window->renderer, 8, 8, 8, 255);
                        SDL_RenderClear(window->renderer);
                        render_stars(window);
                        render_text(window, window->fonts->craft, window->txt[SYNC_CLIENT_SERVER],
                                    white, 150, 150);
                        SDL_RenderPresent(window->renderer);

                        // Wait a frame
                        frame_delay(window->fps);
                    }


                    select_level(window);

                    struct msg msg = { .type = Z_MSG };
                    send_msg(window, &msg);
                    return;

                case 2: // Decline
                    ;
                    struct msg decline_msg = { .type = ACCEPT_MSG };
                    decline_msg.content.boolean = 2;
                    send_msg(window, &decline_msg);
                    break;

                default:
                    break;
            }

            escape = 1;
            begin = SDL_GetTicks();
        }

        // Display black background
        SDL_SetRenderDrawColor(window->renderer, 8, 8, 8, 255);
        SDL_RenderClear(window->renderer);

        // Process/Draw all the things
        render_stars(window);
        render_accept_client_texts(window, begin, selected_item, ip_str);
        SDL_RenderPresent(window->renderer);

        // Wait a frame
        frame_delay(window->fps);
    }
}

static void get_ip(IPaddress *ip, unsigned int *a, unsigned int *b,
                   unsigned int *c, unsigned int *d)
{
    *a = ip->host % 256;
    *b = (ip->host % 65536 - *a) / 256;
    *c = (ip->host % 16777216 - (*a + *b)) / 65536;
    *d = (ip->host % 4294967296 - (*a + *b + *c)) / 16777216;
}





void create_server(struct window *window)
{
    // Create server
    IPaddress server_ip;
    SDLNet_ResolveHost(&server_ip, NULL, 4321);
    window->server = SDLNet_TCP_Open(&server_ip);

    if (!window->server)
        return;

    // Get server IP
    IPaddress *local_ips = xmalloc(sizeof(IPaddress) * MAX_IP_TO_SHOW, window->window, window->renderer);
    int num_ips = SDLNet_GetLocalAddresses(local_ips, MAX_IP_TO_SHOW);

    has_online_ip = 0;
    int escape = 0;
    Uint32 begin = SDL_GetTicks();

    while (!escape)
    {
        window->client = SDLNet_TCP_Accept(window->server);

        if (window->client)
        {
            IPaddress *client_ip = SDLNet_TCP_GetPeerAddress(window->client);

            unsigned int a = 0;
            unsigned int b = 0;
            unsigned int c = 0;
            unsigned int d = 0;
            get_ip(client_ip, &a, &b, &c, &d);

            char buf[40] = { 0 }; // 40 =  Max IPv6 len + 1
            sprintf(buf, "%u.%u.%u.%u", a, b, c, d);

            // Accept client and play
            accept_client(window, buf);

            begin = SDL_GetTicks();
        }

        // Get and handle events
        update_events(window->in, window, 0);
        handle_quit_event(window, 0);
        escape = handle_escape_event(window);

        if (has_online_ip == 0 && window->in->key[SDL_SCANCODE_F7])
        {
            has_online_ip = 1;
            SDL_CreateThread(get_online_ip_thread, "get_online_ip_thread", NULL);
        }

        // Display black background
        SDL_SetRenderDrawColor(window->renderer, 8, 8, 8, 255);
        SDL_RenderClear(window->renderer);

        // Process/Draw all the things
        render_stars(window);

        Uint32 alpha = SDL_GetTicks() - begin;

        if (alpha > TITLE_ALPHA_MAX)
            alpha = TITLE_ALPHA_MAX;
        else if (alpha == 0)
            alpha = 1;

        SDL_Color orange = { 255, 127, 39, alpha };
        SDL_Color white = { 195, 195, 195, alpha };

        render_text(window, window->fonts->zero4b_30_small, window->txt[WAITING_FOR_SOMEONE],
                    orange, 150, 150);

        render_text(window, window->fonts->zero4b_30_extra_small, window->txt[YOUR_IP_LOCAL],
                    white, 150, 300);


        int index = 0;

        for (int i = 0; i < num_ips; i++)
        {
            // Do not display 0.0.0.0
            if (local_ips[i].host == 0)
                continue;

            unsigned int a = 0;
            unsigned int b = 0;
            unsigned int c = 0;
            unsigned int d = 0;
            get_ip(&local_ips[i], &a, &b, &c, &d);

            // Do not display 127.0.0.1
            if (a == 127 && b == 0 && c == 0 && d == 1)
                continue;

            char buf[40] = { 0 }; // 40 =  Max IPv6 len + 1
            sprintf(buf, "%u.%u.%u.%u", a, b, c, d);
            render_text(window, window->fonts->craft,
                        buf, white, 150, 400 + index * 40);

            index++;
        }

        render_text(window, window->fonts->zero4b_30_extra_small, window->txt[YOUR_IP_ONLINE],
                    white, 1060, 300);

        render_text(window, window->fonts->craft, has_online_ip == 2 ? online_ip
                                                  : has_online_ip == 1 ? window->txt[SEARCHING_FOR_ONLINE_IP]
                                                  : window->txt[PRESS_TO_GET_IP], has_online_ip == 0 ? orange : white, 1060, 400);

        render_text(window, window->fonts->craft, window->txt[TO_PLAY_ONLINE_1],
                    white, 150, 800);

        render_text(window, window->fonts->craft, window->txt[TO_PLAY_ONLINE_2],
                    white, 150, 840);

        render_text(window, window->fonts->craft, window->txt[TO_PLAY_ONLINE_3],
                    white, 150, 920);

        SDL_RenderPresent(window->renderer);

        // Wait a frame
        frame_delay(window->fps);
    }

    SDLNet_TCP_Close(window->server);
    window->server = NULL;

    free(local_ips);
}


static int is_char_ipv4(char c)
{
    return (c >= '0' && c <= '9') || c == '.';
}


static void handle_backspace_event(struct window *window, char *buf)
{
    if (window->in->key[SDL_SCANCODE_BACKSPACE])
    {
        window->in->key[SDL_SCANCODE_BACKSPACE] = 0;

        size_t len = strlen(buf);

        if (len > 0)
            buf[len - 1] = '\0';
    }
}

int is_correct_ip(char *str)
{
    // Check if IP != 0.0.0.0
    int is_null_ip = 1;
    for (size_t i = 0; str[i]; i++)
        if (str[i] != '0' && str[i] != '.')
            is_null_ip = 0;

    if (is_null_ip)
        return 0;

    // Check if IP != 255.255.255.255
    size_t i = 0;
    int num_numbers = 0;

    while (str[i])
    {
        if (atoi(str + i) != 255)
            return 1;

        while (str[i] && str[i] != '.')
            i++;

        if (str[i])
            i++;

        num_numbers++;
    }

    return num_numbers != 4;
}


static void reset_global_vars(void)
{
    accepting = 0;
    is_connecting = 0;
    is_connected = 0;
}


void connect_to_server(struct window *window)
{
    int escape = 0;
    Uint32 begin = SDL_GetTicks();
    window->accepted = 0;

    memset(buf, '\0', sizeof(buf));
    memset(err, '\0', sizeof(err));
    reset_global_vars();
    SDL_StartTextInput();

    while (!escape)
    {
        // Get and handle events
        update_events(window->in, window, 0);
        handle_quit_event(window, 0);
        if (!is_connecting && !is_connected)
        {
            escape = handle_escape_event(window);
            handle_backspace_event(window, buf);

            // Copy entered input in buf
            size_t curr_len = strlen(buf);
            size_t to_add_len = strlen(window->in->text);
            size_t index = curr_len;
            for (size_t i = curr_len; i < sizeof(buf) - 1; i++)
            {
                if (i - curr_len >= to_add_len)
                    break;

                if (is_char_ipv4(window->in->text[i - curr_len]))
                {
                    buf[index] = window->in->text[i - curr_len];
                    index++;
                }
            }
            /* Last char is already at '\0' */

            if (handle_play_event(window))
            {
                memset(err, '\0', sizeof(err));
                SDL_CreateThread(connecting_thread, "connecting_thread", window);
            }
        }

        // Display black background
        SDL_SetRenderDrawColor(window->renderer, 8, 8, 8, 255);
        SDL_RenderClear(window->renderer);

        // Process/Draw all the things
        render_stars(window);

        Uint32 alpha = SDL_GetTicks() - begin;

        if (alpha > TITLE_ALPHA_MAX)
            alpha = TITLE_ALPHA_MAX;
        else if (alpha == 0)
            alpha = 1;

        SDL_Color orange = { 255, 127, 39, alpha };
        SDL_Color white = { 195, 195, 195, alpha };
        SDL_Color red = { 255, 0, 0, alpha };
        SDL_Color green = { .r = GREEN_R, .g = GREEN_G, .b = GREEN_B, .a = alpha };

        render_text(window, window->fonts->zero4b_30_small, "Enter IP:",
                    orange, 150, 150);

        int x_offset = 0;

        if (buf[0])
        {
            TTF_SizeText(window->fonts->craft, buf, &x_offset, NULL);
            render_text(window, window->fonts->craft, buf, white, 150, 350);
        }

        if (!is_connecting && !is_connected)
            render_text(window, window->fonts->craft, "_", white, 150 + x_offset, 350);

        if (is_connecting)
        {
            render_text(window, window->fonts->craft,
                        window->txt[CONNECTING], green, 150, 470);
        }

        if (is_connected)
        {
            if (!window->accepted && !accepting)
            {
                accepting = 1;
            }
            else if (!window->accepted && accepting)
            {
                handle_messages(window, "A");
                render_text(window, window->fonts->craft,
                            window->txt[WAITING_FOR_OTHER],
                            green, 150, 470);
            }
            else // if accepted (or declined)
            {
                if (window->accepted == 1) // Do not remove == 1
                {
                    // Query server time
                    struct msg msg = { .type = GET_TIME_MSG };
                    window->client_request_time = SDL_GetTicks();
                    send_msg(window, &msg);

                    lobby(window);
                    begin = SDL_GetTicks();
                }

                reset_global_vars();
                window->accepted = 0;
            }
        }
        else if (err[0])
            render_text(window, window->fonts->craft, err, red, 150, 470);

        SDL_RenderPresent(window->renderer);

        // Wait a frame
        frame_delay(window->fps);
    }

    SDL_StopTextInput();
}



void send_msg(struct window* window, struct msg* msg)
{
    char protocol_msg[MAX_MESSAGE_SIZE] = { 0 };

    switch (msg->type)
    {
    case ACCEPT_MSG:
        SDLNet_Write16(2, protocol_msg);
        protocol_msg[2] = 'A';
        protocol_msg[3] = msg->content.boolean;
        break;

    case RESTART_MSG:
        SDLNet_Write16(5, protocol_msg);
        protocol_msg[2] = 'R';
        SDLNet_Write32(msg->content.ticks, protocol_msg + 3);
        break;

    case MENU_MSG:
        SDLNet_Write16(1, protocol_msg);
        protocol_msg[2] = 'M';
        break;

    case LEVEL_MSG:
        SDLNet_Write16(11, protocol_msg); // strlen("L") + sizeof(Uint16) * 3 + sizeof(Uint32)
        protocol_msg[2] = 'L';
        SDLNet_Write16(msg->content.lvl.level_num, protocol_msg + 3);
        SDLNet_Write16(msg->content.lvl.level_difficulty, protocol_msg + 5);
        SDLNet_Write16(msg->content.lvl.weapon, protocol_msg + 7);
        SDLNet_Write32(msg->content.lvl.start_mission_ticks, protocol_msg + 9);
        break;

    case POSITION_MSG:
        SDLNet_Write16(5, protocol_msg); // strlen("L") + sizeof(Uint16) * 2
        protocol_msg[2] = 'P';
        SDLNet_Write16(msg->content.point.x, protocol_msg + 3);
        SDLNet_Write16(msg->content.point.y, protocol_msg + 5);
        break;

    case SHOOT_MSG:
        SDLNet_Write16(1, protocol_msg); // strlen("L") + sizeof(Uint16) * 2
        protocol_msg[2] = 'S';
        break;

    case BOMB_MSG:
        SDLNet_Write16(1, protocol_msg);
        protocol_msg[2] = 'B';
        break;

    case QUIT_MSG:
        SDLNet_Write16(1, protocol_msg);
        protocol_msg[2] = 'Q';
        break;

    case Z_MSG:
        SDLNet_Write16(1, protocol_msg);
        protocol_msg[2] = 'Z';
        break;

    case GET_TIME_MSG:
        SDLNet_Write16(1, protocol_msg);
        protocol_msg[2] = 'G';
        break;

    case TIME_MSG:
        SDLNet_Write16(5, protocol_msg);
        protocol_msg[2] = 'T';
        SDLNet_Write32(msg->content.ticks, protocol_msg + 3);
        break;

    case SERVER_ALL_MSG:
        ;
        Uint16 size = (Uint16) msg->content.string_vec->size;
        SDLNet_Write16(size + 1, protocol_msg);
        protocol_msg[2] = ':';
        for (size_t i = 0; i < size; i++)
            protocol_msg[i + 3] = msg->content.string_vec->ptr[i];
        break;
    }

    SDLNet_TCP_Send(window->client, protocol_msg, sizeof(Uint16) + SDLNet_Read16(protocol_msg));
    
    if (protocol_msg[2] == 'Z')
        SDLNet_TCP_Send(window->server, protocol_msg, sizeof(Uint16) + SDLNet_Read16(protocol_msg));
}


void recv_msg(struct window *window, char *msg)
{
    char buffer[2] = { 0 };

    if (SDLNet_TCP_Recv(window->client, buffer, 2) <= 0)
    {
        msg[0] = '\0';
        return;
    }

    const Uint16 msg_len = SDLNet_Read16(buffer);

    if (SDLNet_TCP_Recv(window->client, msg, msg_len) <= 0)
    {
        msg[0] = '\0';
        return;
    }

    msg[msg_len] = '\0';
}


static int handle_msg(struct window *window, const char *msg, char *msg_prefixes_to_handle)
{
    if (strchr(msg_prefixes_to_handle, msg[0]))
    {
        switch (msg[0])
        {
            case 'A':
                window->accepted = msg[1];
                break;

            case 'L':
                ;
                window->level_num = SDLNet_Read16(msg + 1);
                window->level_difficulty = SDLNet_Read16(msg + 3);
                window->weapon = SDLNet_Read16(msg + 5);
                window->start_mission_ticks = SDLNet_Read32(msg + 7);
                window->client_time = window->last_sync_time + SDL_GetTicks() - window->ticks;

                return 2;

            case 'R': // Restart when in success/failure screen
                window->restart = 1;
                Uint32 retry_mission_ticks = SDLNet_Read32(msg + 1);

                window->client_time = window->last_sync_time + SDL_GetTicks() - window->ticks;

                waiting_screen(window, SDL_GetTicks() + retry_mission_ticks - window->client_time);
                break;

            case 'M': // Go back to menu when in success/failure screen
                window->restart = 2;
                break;

            case 'P':
                window->player[1].pos.x = SDLNet_Read16(msg + 1);
                window->player[1].pos.y = SDLNet_Read16(msg + 3);
                break;

            case 'S':
                ;
                if (msg[1] == 2)
                    window->player[1].missile_around = 1;

                shoot(window, &window->player[1], 0);
                break;

            case 'B':
                bomb(window, 0);
                window->num_bombs--;
                break;

            case 'Q': // Quit level
                quit_game(window);
                return 0;

            case 'Z': // Quit online
                return 0;

            case 'G':
                ;
                struct msg send_time_msg = { .type = TIME_MSG };
                send_time_msg.content.ticks = SDL_GetTicks();
                send_msg(window, &send_time_msg);
                return 0;

            case 'T':
                ;
                Uint32 server_time = SDLNet_Read32(msg + 1);
                window->ticks = SDL_GetTicks();
                Uint32 latency = (window->ticks - window->client_request_time) / 2;
                window->last_sync_time = server_time + latency;
                break;

            case ':': // SERVER_ALL_MSG
                for (enum list_type i = 0; i < NUM_LISTS; i++)
                    clear_list(window->list[i]);

                int i = 1;
                while (msg[i])
                {
                    struct list* new = xmalloc(sizeof(struct list), window->window, window->renderer);
                    char* msg_begin = msg + i;
                    enum list_type type;

                    if (msg_begin[0] == 'e')
                    {
                        float pos_x = read_float(msg_begin + 1);
                        float pos_y = read_float(msg_begin + 5);
                        SDL_FRect pos = {.x = pos_x, .y = pos_y };
                        float max_health = SDLNet_Read16(msg_begin + 9);
                        float health = SDLNet_Read16(msg_begin + 11);
                        float speed_x = read_float(msg_begin + 13);
                        float speed_y = read_float(msg_begin + 17);
                        char enemy_type = msg_begin[21];

                        set_enemy_attributes(new, &pos, window, enemy_type, speed_x, health, max_health, 1, speed_y);

                        type = ENEMY_LIST;
                        i += 22;
                    }
                    else if (msg_begin[0] == 's')
                    {
                        float pos_x = read_float(msg_begin + 1);
                        float pos_y = read_float(msg_begin + 5);
                        SDL_FRect pos = { .x = pos_x, .y = pos_y };
                        set_shot_pos(new, &pos, window, 1);

                        type = MY_SHOTS_LIST;
                        i += 9;
                    }
                    else if (msg_begin[0] == 't')
                    {
                        float pos_x = read_float(msg_begin + 1);
                        float pos_y = read_float(msg_begin + 5);
                        SDL_FRect pos = { .x = pos_x, .y = pos_y };
                        float speed_x = read_float(msg_begin + 9);
                        float speed_y = read_float(msg_begin + 13);
                        SDL_FRect speed = { .x = speed_x, .y = speed_y };
                        char shot_type = msg_begin[17];
                        set_enemy_shot_attributes(new, &pos, NULL, shot_type, window, &speed, 1);

                        type = ENEMY_SHOT_LIST;
                        i += 18;
                    }
                    else if (msg_begin[0] == 'o') // Object
                    {
                        float pos_x = read_float(msg_begin + 1);
                        float pos_y = read_float(msg_begin + 5);
                        SDL_FRect pos = { .x = pos_x, .y = pos_y };
                        enum object_type object_type = msg_begin[9];

                        set_object_attributes(new, object_type, window->img->objects[object_type], &pos);

                        type = OBJECT_LIST;
                        i += 10;
                    }
                    else if (msg_begin[0] == 'x')
                    {
                        float pos_x = read_float(msg_begin + 1);
                        float pos_y = read_float(msg_begin + 5);
                        SDL_FRect pos = { .x = pos_x, .y = pos_y };

                        float pos_src_x = read_float(msg_begin + 9);
                        float pos_src_y = read_float(msg_begin + 13);
                        float pos_src_w = read_float(msg_begin + 17);
                        float pos_src_h = read_float(msg_begin + 21);
                        SDL_FRect pos_src = { .x = pos_src_x, .y = pos_src_y, .w = pos_src_w, .h = pos_src_h };
                        int explosion_type = msg_begin[25];

                        set_explosion_pos(new, window, &pos, NULL, explosion_type, &pos_src);

                        type = EXPLOSION_LIST;
                        i += 26;
                    }
                    else if (msg_begin[0] == 'h')
                    {
                        float pos_x = read_float(msg_begin + 1);
                        float pos_y = read_float(msg_begin + 5);
                        float pos_w = read_float(msg_begin + 9);
                        float pos_h = read_float(msg_begin + 13);
                        SDL_FRect pos = { .x = pos_x, .y = pos_y, .w = pos_w, .h = pos_h };

                        Uint32 time_diff = SDLNet_Read32(msg_begin + 17);
                     
                        set_hud_text(new, &pos, window, 1, SDL_GetTicks() - time_diff);

                        type = HUD_LIST;
                        i += 21;
                    }
                    else if (msg_begin[0] == 'b')
                    {
                        float pos_x = read_float(msg_begin + 1);
                        float pos_y = read_float(msg_begin + 5);
                        float pos_w = read_float(msg_begin + 9);
                        float pos_h = read_float(msg_begin + 13);
                        SDL_FRect pos = { .x = pos_x, .y = pos_y, .w = pos_w, .h = pos_h };
                        float max_health = SDLNet_Read16(msg_begin + 17);
                        float health = SDLNet_Read16(msg_begin + 19);
                        float speed_x = read_float(msg_begin + 21);
                        float speed_y = read_float(msg_begin + 25);
                        SDL_FRect speed = { .x = speed_x, .y = speed_y };
                        char enemy_type = msg_begin[29];

                        set_boss_attributes(new, &pos, window, enemy_type, 1, &speed, health, max_health);

                        type = BOSS_LIST;
                        i += 30;
                    }

                    // Pushing element in front of the correct list
                    new->next = window->list[type]->next;
                    window->list[type]->next = new;
                }
                break;
        }
    }

    return 1;
}


int handle_messages(struct window *window, char *msg_prefixes_to_handle)
{
    struct msg_list *curr_msg = window->msg_list->next;

    while (curr_msg)
    {
        int res = handle_msg(window, curr_msg->msg, msg_prefixes_to_handle);

        if (!res || res == 2)
        {
            clear_msg_list(window->msg_list);
            return res;
        }

        curr_msg = curr_msg->next;
    }

    clear_msg_list(window->msg_list);

    return 1;
}

int connecting_thread(void *data)
{
    is_connecting = 1;

    struct window *window = data;
    IPaddress ip;

    if (is_correct_ip(buf))
    {
        if (SDLNet_ResolveHost(&ip, buf, 4321) == 0)
        {
            window->client = SDLNet_TCP_Open(&ip);

            if (window->client)
            {
                is_connected = 1;
                SDL_CreateThread(recv_thread, "recv_thread", window);
            }
            else
                strcpy(err, SDLNet_GetError());
        }
        else
            strcpy(err, SDLNet_GetError());
    }
    else
        strcpy(err, window->txt[INVALID_IP]);

    is_connecting = 0;
    return 0;
}



int recv_thread(void *data)
{
    struct window *window = data;
    char msg[MAX_MESSAGE_SIZE] = { 0 };

    while (1)
    {
        recv_msg(window, msg);
           
        if (msg[0])
        {
            if (msg[0] == 'Z')
            {
                SDLNet_TCP_Close(window->client);
                window->client = NULL;
                break;
            }
            else
                add_to_msg_list(window, window->msg_list, msg);
        }    
        else
            break;
    }

    return 0;
}


int get_online_ip_thread(void *data)
{
    (void) data;

    online_ip = get_online_ip();
    has_online_ip = 2;

    return 0;
}

float read_float(char* buffer)
{
    Uint32 data = SDLNet_Read32(buffer);
    float res = 0.0;
    memcpy(&res, &data, sizeof(data));
    return res;
}

void write_float(float number, char* buffer_to_write_to)
{
    Uint32 unsigned_num = 0;
    memcpy(&unsigned_num, &number, sizeof(number));
    SDLNet_Write32(unsigned_num, buffer_to_write_to);
}

