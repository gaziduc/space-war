#ifndef MSG_LIST
#define MSG_LIST

#include <stddef.h>

#define MAX_MESSAGE_SIZE 16384

/* Extra zeroed bytes after each message so that fixed-size reads never go out of bounds */
#define MSG_PADDING 64

struct msg_list
{
    struct msg_list *next;
    size_t len;
    char msg[]; // len + MSG_PADDING bytes, zero-padded
};

/* Called once before any thread uses the list */
void init_msg_list_lock(void);

/* Thread safe: can be called by the receiving thread */
void add_to_msg_list(struct window *window, struct msg_list *msg_list, const char *msg, size_t len);

/* Thread safe: detaches all the pending messages and returns them (NULL if none).
 * The caller owns the returned chain and must free it with free_msg_chain. */
struct msg_list *take_msg_list(struct msg_list *msg_list);

void free_msg_chain(struct msg_list *chain);

void clear_msg_list(struct msg_list *msg_list);

#endif // MSG_LIST
