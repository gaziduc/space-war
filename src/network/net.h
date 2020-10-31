#ifndef NET_H
#define NET_H

#define MAX_IP_TO_SHOW 10

void create_or_join(struct window *window);

void create_server(struct window *window);

void connect_to_server(struct window *window);

void send_msg(struct window *window, struct msg *msg);

void recv_msg(struct window *window, char *msg);

int handle_messages(struct window *window, char *msg_prefixes_to_handle);

int connecting_thread(void *data);

int recv_thread(void *data);

int get_online_ip_thread(void *data);

#endif /* !NET_H */
