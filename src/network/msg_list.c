#include "init.h"
#include "msg_list.h"
#include "utils.h"

void add_to_msg_list(struct window *window, struct msg_list *msg_list, char msg[MAX_MESSAGE_SIZE])
{
    struct msg_list *temp = msg_list;

    while (temp->next)
        temp = temp->next;

    temp->next = xmalloc(sizeof(struct msg_list), window->window, window->renderer);

    for (size_t i = 0; i < MAX_MESSAGE_SIZE; i++)
        temp->next->msg[i] = msg[i];

    temp->next->next = NULL;
}

static void clear_msg_list_rec(struct msg_list *msg_list)
{
    if (msg_list->next)
        clear_msg_list_rec(msg_list->next);

    free(msg_list);
}

void clear_msg_list(struct msg_list *msg_list)
{
    if (msg_list->next)
        clear_msg_list_rec(msg_list->next);

    msg_list->next = NULL;
}
