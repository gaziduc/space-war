#include "init.h"
#include "msg_list.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>

static SDL_mutex *msg_lock = NULL;

void init_msg_list_lock(void)
{
    if (!msg_lock)
        msg_lock = SDL_CreateMutex();
}

void add_to_msg_list(struct window *window, struct msg_list *msg_list, const char *msg, size_t len)
{
    if (len > MAX_MESSAGE_SIZE)
        return;

    struct msg_list *node = calloc(1, sizeof(struct msg_list) + len + MSG_PADDING);

    if (!node)
        return; // Out of memory: drop the message instead of crashing from a thread

    (void) window;

    node->len = len;
    memcpy(node->msg, msg, len);
    node->next = NULL;

    SDL_LockMutex(msg_lock);

    struct msg_list *temp = msg_list;
    while (temp->next)
        temp = temp->next;
    temp->next = node;

    SDL_UnlockMutex(msg_lock);
}

struct msg_list *take_msg_list(struct msg_list *msg_list)
{
    SDL_LockMutex(msg_lock);

    struct msg_list *chain = msg_list->next;
    msg_list->next = NULL;

    SDL_UnlockMutex(msg_lock);

    return chain;
}

void free_msg_chain(struct msg_list *chain)
{
    // Iterative on purpose: a recursive free overflows the stack on long chains
    while (chain)
    {
        struct msg_list *next = chain->next;
        free(chain);
        chain = next;
    }
}

void clear_msg_list(struct msg_list *msg_list)
{
    free_msg_chain(take_msg_list(msg_list));
}
