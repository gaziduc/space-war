#ifndef IP_H
#define IP_H

struct string
{
    char *ptr;
    size_t len;
};

int init_string(struct string *str);

size_t write_function(void *ptr, size_t size, size_t nmemb, struct string *s);

const char* get_online_ip(void);

#endif // IP_H
