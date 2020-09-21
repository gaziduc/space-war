#ifndef IP_H
#define IP_H

struct string
{
    char *ptr;
    size_t len;
};

const char* get_online_ip(void);

#endif // IP_H
