#ifndef NET_H
#define NET_H

void create_or_join(struct window *window);

void create_server(struct window *window);

void connect_to_server(struct window *window);

int connecting_thread(void *data);

#endif /* !NET_H */
