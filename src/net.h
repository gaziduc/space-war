#ifndef NET_H
#define NET_H

struct state
{
    Uint16 pos_x;
    Uint16 pos_y;
    Uint16 health;
    Uint16 ammo;
    char is_shooting;
    char throw_bomb;
    char has_shield;
    char quit;
};

void create_or_join(struct window *window);

void create_server(struct window *window);

void connect_to_server(struct window *window);

void send_state(struct player *player, struct window *window,
                char is_shooting, char throw_bomb, char has_shield, char quit);

void recv_state(struct window *window, struct state *state);

int connecting_thread(void *data);

int waiting_thread(void *data);

#endif /* !NET_H */
