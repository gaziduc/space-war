#ifndef VERSION_H
#define VERSION_H

    #define VERSION_MAJOR   1
    #define VERSION_MINOR   10
    #define VERSION_PATCH   0

    #define VERSION_WIN     VERSION_MAJOR,VERSION_MINOR,VERSION_PATCH,0
    #define VERSION_NUM     VERSION_MAJOR.VERSION_MINOR.VERSION_PATCH\0
    #define str(n)          #n
    #define xstr(n)         str(n)
    #define VERSION_STR     xstr(VERSION_NUM)
    #define DESCRIPTION_STR xstr(Space War VERSION_NUM)
    #define VERSION_INGAME  xstr(VER VERSION_NUM)

#endif // VERSION_H
