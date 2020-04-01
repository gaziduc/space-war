#include "init.h"
#include "menu.h"
#include "event.h"
#include "utils.h"
#include "net.h"
#include "level.h"
#include "game.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_net.h>


char buf[40]; // 40 = Max IPv6 len + 1
int is_connecting;
int is_connected;
char err[256];
int level_num;
int level_difficulty;
int selecting;
int selected;


static void render_create_or_join_texts(struct window *window, Uint32 begin, int selected_item)
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
    render_text(window, window->fonts->zero4b_30_small, "CREATE OR JOIN ?", orange, 150, 150);

    // Render items
    char *s_list[2] = { "-> CREATE", "-> JOIN" };

    for (int i = 1; i <= 2; i++)
    {
        if (selected_item != i)
            render_text(window, window->fonts->zero4b_30_small, s_list[i - 1] + 3, blue,
                        150, 450 + (i - 1) * 100);
        else
            render_text(window, window->fonts->zero4b_30_small, s_list[i - 1], green,
                        150, 450 + (i - 1) * 100);
    }
}


void create_or_join(struct window *window)
{
    int escape = 0;
    int selected_item = 1;
    Uint32 begin = SDL_GetTicks();

    while (!escape)
    {
        // Get and handle events
        update_events(window->in, window);
        handle_quit_event(window, 0);
        handle_select_arrow_event(window, &selected_item, 2);
        escape = handle_escape_event(window);

        if (handle_play_event(window))
        {
            switch (selected_item)
            {
                case 1: // Create
                    create_server(window);
                    break;

                case 2: // Join
                    connect_to_server(window);
                    break;

                default:
                    break;
            }

            begin = SDL_GetTicks();
        }

        // Display black background
        SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
        SDL_RenderClear(window->renderer);

        // Process/Draw all the things
        render_stars(window);
        render_create_or_join_texts(window, begin, selected_item);
        SDL_RenderPresent(window->renderer);

        // Wait a frame
        SDL_framerateDelay(window->fps);
    }
}


static void render_accept_client_texts(struct window *window, Uint32 begin, int selected_item,
                                       char *ip_str)
{
    Uint32 alpha = SDL_GetTicks() - begin;

    if (alpha > TITLE_ALPHA_MAX)
        alpha = TITLE_ALPHA_MAX;
    else if (alpha == 0)
        alpha = 1;

    SDL_Color blue = { .r = 0, .g = 255, .b = 255, .a = alpha };
    SDL_Color green = { .r = 0, .g = 255, .b = 0, .a = alpha };
    SDL_Color orange = { .r = 255, .g = 128, .b = 0, .a = alpha };

    char text[100] = { 0 };
    sprintf(text, "User at %s wants to play with you.", ip_str);

    // Render title
    render_text(window, window->fonts->zero4b_30_extra_small, text, orange, 150, 150);

    // Render items
    char *s_list[2] = { "-> ACCEPT", "-> DECLINE" };

    for (int i = 1; i <= 2; i++)
    {
        if (selected_item != i)
            render_text(window, window->fonts->zero4b_30_small, s_list[i - 1] + 3, blue,
                        150, 450 + (i - 1) * 100);
        else
            render_text(window, window->fonts->zero4b_30_small, s_list[i - 1], green,
                        150, 450 + (i - 1) * 100);
    }
}




static void accept_client(struct window *window, char *ip_str)
{
    int escape = 0;
    int selected_item = 1;
    Uint32 begin = SDL_GetTicks();

    while (!escape)
    {
        // Get and handle events
        update_events(window->in, window);
        handle_quit_event(window, 0);
        handle_select_arrow_event(window, &selected_item, 2);
        escape = handle_escape_event(window);

        if (handle_play_event(window))
        {
            switch (selected_item)
            {
                case 1: // Accept
                    select_level(window);
                    break;

                case 2: // Decline
                    escape = 1;
                    break;

                default:
                    break;
            }

            begin = SDL_GetTicks();
        }

        // Display black background
        SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
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

    // Get server IP
    IPaddress *local_ips = xmalloc(sizeof(IPaddress) * 12, window->window);
    int num_ips = SDLNet_GetLocalAddresses(local_ips, 12);

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

            // Then close socket
            SDLNet_TCP_Close(window->client);
            window->client = NULL;
        }

        // Get and handle events
        update_events(window->in, window);
        handle_quit_event(window, 0);
        escape = handle_escape_event(window);

        // Display black background
        SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
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

        render_text(window, window->fonts->zero4b_30_small, "Waiting for someone...",
                    orange, 150, 150);

        render_text(window, window->fonts->zero4b_30_extra_small, "Your IP:",
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
                        buf, white, 150, 400 + index * 50);

            index++;
        }

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


void connect_to_server(struct window *window)
{
    int escape = 0;
    Uint32 begin = SDL_GetTicks();

    memset(buf, '\0', sizeof(buf));
    memset(err, '\0', sizeof(err));
    level_num = 0;
    level_difficulty = 0;
    selected = 0;
    selecting = 0;

    SDL_StartTextInput();

    while (!escape)
    {
        // Get and handle events
        update_events(window->in, window);
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
                SDL_CreateThread(connecting_thread, "connecting_thread", window);
        }

        // Display black background
        SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
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
            render_text(window, window->fonts->pixel, "_", white, 150 + strlen(buf) * 18, 350);

        if (is_connecting)
            render_text(window, window->fonts->pixel, "Connecting... Please wait...", green, 150, 470);

        if (is_connected)
        {
            if (!selected && !selecting)
            {
                SDL_CreateThread(waiting_thread, "waiting_thread", window);
                selecting = 1;
            }
            else if (!selected && selecting)
            {
                render_text(window, window->fonts->pixel,
                         "Connected! Waiting for the other to choose mission...",
                         green, 150, 470);

                selected = 0;
            }
            else // if selected
            {
                play_game(window, level_num, level_difficulty);
                selecting = 1;
                selected = 0;
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


void send_state(struct player *player, struct window *window, char is_shooting, char has_shield)
{
    char data[8] = { 0 }; // 8 = 3 * sizeof(Uin16) + 2 * sizeof(char)

    SDLNet_Write16((Uint16) player->pos.x, data);
    SDLNet_Write16((Uint16) player->pos.y, data + 2);
    SDLNet_Write16((Uint16) player->health, data + 4);
    data[6] = is_shooting;
    data[7] = has_shield;

    SDLNet_TCP_Send(window->client, data, sizeof(data));
}


void recv_state(struct window *window, struct state *state)
{
    char data[8] = { 0 }; // 8 = 3 * sizeof(Uin16) + 2 * sizeof(char)

    SDLNet_TCP_Recv(window->client, data, sizeof(data));

    state->pos_x = SDLNet_Read16(data);
    state->pos_y = SDLNet_Read16(data + 2);
    state->health = SDLNet_Read16(data + 4);
    state->is_shooting = data[6];
    state->has_shield = data[7];
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
                is_connected = 1;
            else
                strcpy(err, SDLNet_GetError());
        }
        else
            strcpy(err, SDLNet_GetError());
    }
    else
        strcpy(err, "Invalid IP");

    is_connecting = 0;
    return 0;
}


int waiting_thread(void *data)
{
    struct window *window = data;
    char data_received[2] = { 0 };

    SDLNet_TCP_Recv(window->client, data_received, sizeof(data_received));
    level_num = data_received[0];
    level_difficulty = data_received[1];
    selected = 1;

    return 0;
}
