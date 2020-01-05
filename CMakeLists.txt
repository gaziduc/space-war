cmake_minimum_required(VERSION 3.0)
project(space-war)

aux_source_directory(src SRC)

if (MSVC)
    add_compile_options(/W4 /WX)
else ()
    add_compile_options(-Wall -Wextra -Werror -pedantic -std=c99 -O2)
endif ()

add_link_options(-lm -lSDL2 -lSDL2_image -lSDL2_gfx -lSDL2_ttf -lSDL2_mixer)

add_executable(${PROJECT_NAME} ${SRC})
