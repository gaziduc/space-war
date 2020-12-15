#include "utils.h"
#include "ip.h"
#include <string.h>
#include <curl/curl.h>

int init_string(struct string *str)
{
    str->len = 0;

    str->ptr = malloc(str->len + 1);
    if (!str->ptr)
        return 0;

    str->ptr[0] = '\0';
    return 1;
}

size_t write_function(void *ptr, size_t size, size_t nmemb, struct string *s)
{
    size_t new_len = s->len + size * nmemb;

    s->ptr = realloc(s->ptr, new_len + 1);
    memcpy(s->ptr + s->len, ptr, size * nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;

    return size * nmemb;
}

const char* get_online_ip(void)
{
    static char res[40] = { 0 };

    CURL *curl = curl_easy_init();

    if (!curl)
        return "Could not find online IP. Curl init failed.";

    struct string str;
    if (!init_string(&str))
        return "Could not find online IP. Malloc failed.";

    curl_easy_setopt(curl, CURLOPT_URL, "http://ifconfig.me");

    // if website is redirected, we tell libcurl to follow redirection
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_function);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &str);

    CURLcode return_code = curl_easy_perform(curl);

    if (return_code != CURLE_OK)
        return curl_easy_strerror(return_code);

    strncpy(res, str.ptr, 39);
    free(str.ptr);

    curl_easy_cleanup(curl);

    return res;
}
