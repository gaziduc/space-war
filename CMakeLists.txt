cmake_minimum_required(VERSION 3.13)
project(space-war C)

# source files
file(GLOB_RECURSE SRC "src/*.c")

# dependencies headers
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(SDL2 REQUIRED)
find_package(SDL2_IMAGE REQUIRED)
find_package(SDL2_TTF REQUIRED)
find_package(SDL2_MIXER REQUIRED)
find_package(SDL2_GFX REQUIRED)
find_package(SDL2_NET REQUIRED)

if (WIN32)
    include_directories(SRC ${SDL2_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIR} ${SDL2_MIXER_INCLUDE_DIR} ${SDL2_GFX_INCLUDE_DIR} ${SDL2_NET_INCLUDE_DIR} ${PROJECT_SOURCE_DIR}/dep/win64-mingw/Curl/include)
else ()
    find_package(CURL REQUIRED)
    include_directories(SRC ${SDL2_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIR} ${SDL2_MIXER_INCLUDE_DIR} ${SDL2_GFX_INCLUDE_DIR} ${SDL2_NET_INCLUDE_DIR} ${CURL_INCLUDE_DIRS})
endif ()



# Warnings
if (MSVC)
    add_compile_options(/W3)
else ()
    add_compile_options(-Wall -Wextra -pedantic -std=c99 -O2)
endif ()

# compile
if (WIN32)
    add_executable(${PROJECT_NAME} ${SRC} icon/res.rc)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/dep/win64-mingw/bin $<TARGET_FILE_DIR:${PROJECT_NAME}>/)
else ()
    add_executable(${PROJECT_NAME} ${SRC})
endif ()

# local includes
target_include_directories(${PROJECT_NAME} PRIVATE "src" "src/data_struct" "src/game" "src/init" "src/menus" "src/network" "src/settings" "src/utils")

# dependencies libraries
if (WIN32)
    target_link_libraries(${PROJECT_NAME} ${SDL2MAIN_LIBRARY} ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${SDL2_MIXER_LIBRARY} ${SDL2_GFX_LIBRARY} ${SDL2_NET_LIBRARY} ${PROJECT_SOURCE_DIR}/dep/win64-mingw/Curl/lib/libcurl_imp.lib)
else ()
    target_link_libraries(${PROJECT_NAME} ${SDL2MAIN_LIBRARY} ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${SDL2_MIXER_LIBRARY} ${SDL2_GFX_LIBRARY} ${SDL2_NET_LIBRARY} ${CURL_LIBRARIES})
endif ()

# Other
if (MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS /SUBSYSTEM:WINDOWS)
else ()
    target_link_libraries(${PROJECT_NAME} m)
endif ()
