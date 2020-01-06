cmake_minimum_required(VERSION 3.7)
project(space-war)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(SDL2 REQUIRED)
find_package(SDL2_IMAGE REQUIRED)
find_package(SDL2_TTF REQUIRED)
find_package(SDL2_MIXER REQUIRED)
find_package(SDL2_GFX REQUIRED)

aux_source_directory(src SRC)

include_directories(SRC ${SDL2_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIR} ${SDL2_MIXER_INCLUDE_DIR} ${SDL2_GFX_INCLUDE_DIR})

if (MSVC)
    add_compile_options(/W3)
else ()
    add_compile_options(-Wall -Wextra -Werror -pedantic -std=c99 -O2)
endif ()

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} ${SDL2MAIN_LIBRARY} ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${SDL2_MIXER_LIBRARY} ${SDL2_GFX_LIBRARY})

if (MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS /SUBSYSTEM:WINDOWS)
else ()
    add_link_options(-lm)
endif ()