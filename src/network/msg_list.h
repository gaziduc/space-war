#ifndef MSG_LIST
#define MSG_LIST

struct msg_list
{
    char msg[128];
    struct msg_list *next;
};

void add_to_msg_list(struct window *window, struct msg_list *msg_list, char *msg);

void clear_msg_list(struct msg_list *msg_list);

#endif // MSG_LIST
