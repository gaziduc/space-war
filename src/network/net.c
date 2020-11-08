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
#include <stdio.h>
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
char *online_ip;


static void render_create_or_join_texts(struct window *window, Uint32 begin,
                                        int selected_item)
{
    Uint32 alpha = SDL_GetTicks() - begin;

    if (alpha > TITLE_ALPHA_MAX)
        alpha = TITLE_ALPHA_MAX;
    else if (alpha == 0)
        alpha = 1;

    SDL_Color blue = { .r = 0, .g = 255, .b = 255, .a = alpha };
    SDL_Color green = { .r = 0, .g = 255, .b = 0, .a = alpha };
    SDL_Color orange = { .r = 255, .g = 128, .b = 0, .a = alpha };

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
        SDL_RenderPresent(window->renderer);

        // Wait a frame
        SDL_framerateDelay(window->fps);
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

    SDL_Color blue = { .r = 0, .g = 255, .b = 255, .a = alpha };
    SDL_Color green = { .r = 0, .g = 255, .b = 0, .a = alpha };
    SDL_Color orange = { .r = 255, .g = 128, .b = 0, .a = alpha };

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
                    ;
                    struct msg accept_msg = { .type = ACCEPT_MSG };
                    accept_msg.content.boolean = 1;
                    send_msg(window, &accept_msg);

                    SDL_CreateThread(recv_thread, "recv_thread", window);

                    while (handle_messages(window, "G"))
                        SDL_Delay(10);

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
        SDL_framerateDelay(window->fps);
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

        SDL_Color orange = { .r = 255, .g = 128, .b = 0, .a = alpha };
        SDL_Color white = { 255, 255, 255, alpha };

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
            render_text(window, window->fonts->pixel,
                        buf, white, 150, 400 + index * 40);

            index++;
        }

        render_text(window, window->fonts->zero4b_30_extra_small, window->txt[YOUR_IP_ONLINE],
                    white, 1060, 300);

        render_text(window, window->fonts->pixel, has_online_ip == 2 ? online_ip
                                                  : has_online_ip == 1 ? window->txt[SEARCHING_FOR_ONLINE_IP]
                                                  : window->txt[PRESS_TO_GET_IP], has_online_ip == 0 ? orange : white, 1060, 400);

        render_text(window, window->fonts->pixel, window->txt[TO_PLAY_ONLINE_1],
                    white, 150, 800);

        render_text(window, window->fonts->pixel, window->txt[TO_PLAY_ONLINE_2],
                    white, 150, 840);

        render_text(window, window->fonts->pixel, window->txt[TO_PLAY_ONLINE_3],
                    white, 150, 920);

        SDL_RenderPresent(window->renderer);

        // Wait a frame
        SDL_framerateDelay(window->fps);
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

        SDL_Color orange = { 255, 128, 0, alpha };
        SDL_Color white = { 255, 255, 255, alpha };
        SDL_Color red = { 255, 0, 0, alpha };
        SDL_Color green = { 0, 255, 0, alpha };

        render_text(window, window->fonts->zero4b_30_small, "Enter IP:",
                    orange, 150, 150);

        if (buf[0])
            render_text(window, window->fonts->pixel, buf, white, 150, 350);

        if (!is_connecting && !is_connected)
            render_text(window, window->fonts->pixel, "_",
                        white, 150 + strlen(buf) * 18, 350);

        if (is_connecting)
        {
            render_text(window, window->fonts->pixel,
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
                render_text(window, window->fonts->pixel,
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
            render_text(window, window->fonts->pixel, err, red, 150, 470);

        SDL_RenderPresent(window->renderer);

        // Wait a frame
        SDL_framerateDelay(window->fps);
    }

    SDL_StopTextInput();
}



void send_msg(struct window *window, struct msg *msg)
{
    char protocol_msg[128] = { 0 };

    switch (msg->type)
    {
        case ACCEPT_MSG:
            protocol_msg[0] = 2;
            protocol_msg[1] = 'A';
            protocol_msg[2] = msg->content.boolean;
            break;

        case RESTART_MSG:
            protocol_msg[0] = 5;
            protocol_msg[1] = 'R';
            SDLNet_Write32(msg->content.ticks, protocol_msg + 2);
            break;

        case MENU_MSG:
            protocol_msg[0] = 1;
            protocol_msg[1] = 'M';
            break;

        case LEVEL_MSG:
            protocol_msg[0] = 11; // strlen("L") + sizeof(Uint16) * 3
            protocol_msg[1] = 'L';
            SDLNet_Write16(msg->content.lvl.level_num, protocol_msg + 2);
            SDLNet_Write16(msg->content.lvl.level_difficulty, protocol_msg + 4);
            SDLNet_Write16(msg->content.lvl.weapon, protocol_msg + 6);
            SDLNet_Write32(msg->content.lvl.start_mission_ticks, protocol_msg + 8);
            break;

        case POSITION_MSG:
            protocol_msg[0] = 5; // strlen("L") + sizeof(Uint16) * 2
            protocol_msg[1] = 'P';
            SDLNet_Write16(msg->content.point.x, protocol_msg + 2);
            SDLNet_Write16(msg->content.point.y, protocol_msg + 4);
            break;

        case SHOOT_MSG:
            protocol_msg[0] = 1; // strlen("L") + sizeof(Uint16) * 2
            protocol_msg[1] = 'S';
            break;

        case BOMB_MSG:
            protocol_msg[0] = 1;
            protocol_msg[1] = 'B';
            break;

        case QUIT_MSG:
            protocol_msg[0] = 1;
            protocol_msg[1] = 'Q';
            break;

        case Z_MSG:
            protocol_msg[0] = 1;
            protocol_msg[1] = 'Z';
            break;

        case GET_TIME_MSG:
            protocol_msg[0] = 1;
            protocol_msg[1] = 'G';
            break;

        case TIME_MSG:
            protocol_msg[0] = 5;
            protocol_msg[1] = 'T';
            SDLNet_Write32(msg->content.ticks, protocol_msg + 2);
            break;
    }

    SDLNet_TCP_Send(window->client, protocol_msg, 1 + protocol_msg[0]);

    if (protocol_msg[1] == 'Z')
        SDLNet_TCP_Send(window->server, protocol_msg, 1 + protocol_msg[0]);
}


void recv_msg(struct window *window, char *msg)
{
    Uint8 msg_len = 0;

    SDLNet_TCP_Recv(window->client, &msg_len, sizeof(msg_len));
    SDLNet_TCP_Recv(window->client, msg, msg_len);

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
                Uint16 level_num = SDLNet_Read16(msg + 1);
                Uint16 level_difficulty = SDLNet_Read16(msg + 3);
                window->weapon = SDLNet_Read16(msg + 5);
                Uint32 start_mission_ticks = SDLNet_Read32(msg + 7);

                window->client_time = window->last_sync_time + SDL_GetTicks() - window->ticks;

                waiting_screen(window, SDL_GetTicks() + start_mission_ticks - window->client_time);

                play_game(window, level_num, level_difficulty);
                break;

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
                free_background(window->stars);
                free_vector(window->paths);
                window->paths = NULL; // important, see free_all in free.c
                load_music(window, "data/endgame.ogg", 1);
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
        }
    }

    return 1;
}


int handle_messages(struct window *window, char *msg_prefixes_to_handle)
{
    struct msg_list *curr_msg = window->msg_list->next;

    while (curr_msg)
    {
        if (!handle_msg(window, curr_msg->msg, msg_prefixes_to_handle))
        {
            clear_msg_list(window->msg_list);
            return 0;
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
    char msg[128] = { 0 };

    do
    {
        recv_msg(window, msg);
        add_to_msg_list(window, window->msg_list, msg);
    } while (msg[0] != 'Z');

    if (window->server)
    {
        SDLNet_TCP_Close(window->client);
        window->client = NULL;
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

