#ifndef NET_H
#define NET_H

#define MAX_IP_TO_SHOW 10

void create_or_join(struct window *window);

void create_server(struct window *window);

void connect_to_server(struct window *window);

void send_state(struct player *player, struct window *window,
                char is_shooting, char throw_bomb, char has_shield,
                char state, char level_num, char level_difficulty);

void recv_state(struct window *window, struct state *state);

int connecting_thread(void *data);

int accepting_thread(void *data);

int get_online_ip_thread(void *data);

#endif /* !NET_H */
