cmake_minimum_required(VERSION 3.13)
project(space-war C)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# source files
file(GLOB_RECURSE SRC "src/*.c" "src/*.h")

# keep folder tree for Visual Studio
if (MSVC)
    foreach (FILE ${SRC})
        # Get the directory of the source file
        get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)

        # Remove common directory prefix to make the group
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")

        # Make sure we are using windows slashes
        string(REPLACE "/" "\\" GROUP "${GROUP}")

        # Group into "Source Files" and "Header Files"
        if ("${FILE}" MATCHES ".*\\.c")
            set(GROUP "Source Files${GROUP}")
        elseif ("${FILE}" MATCHES ".*\\.h")
            set(GROUP "Header Files${GROUP}")
        endif ()

        source_group("${GROUP}" FILES "${FILE}")
    endforeach ()
endif ()


# Warning: put this before find_package as they use DEPS_DIR on Windows
if (WIN32)
    if (MSVC)
        set(DEPS_DIR ${PROJECT_SOURCE_DIR}/dep/win64-msvc)
    else ()
        set(DEPS_DIR ${PROJECT_SOURCE_DIR}/dep/win64-mingw)
    endif ()
endif ()


if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(USE_FLAGS "-O3 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='[\"png\"]' -s USE_SDL_MIXER=2 -s SDL2_MIXER_FORMATS='[\"ogg\"]' -s USE_SDL_TTF=2 -s USE_SDL_GFX=2 -s USE_SDL_NET=2 -s ALLOW_MEMORY_GROWTH=1 -s ASYNCIFY --preload-file data -s WASM=1 --shell-file template.html --post-js postfix.js")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USE_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USE_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${USE_FLAGS}")
    set(CMAKE_EXECUTABLE_SUFFIX .html)
else ()
    # dependencies headers
    set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

    find_package(SDL2 REQUIRED)
    find_package(SDL2_IMAGE REQUIRED)
    find_package(SDL2_TTF REQUIRED)
    find_package(SDL2_MIXER REQUIRED)
    find_package(SDL2_GFX REQUIRED)
    find_package(SDL2_NET REQUIRED)

    if (WIN32)
        include_directories(SRC ${SDL2_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIR} ${SDL2_MIXER_INCLUDE_DIR} ${SDL2_GFX_INCLUDE_DIR} ${SDL2_NET_INCLUDE_DIR} ${DEPS_DIR}/Curl/x64/include)
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
endif ()

# compile
 if (WIN32)
    add_executable(${PROJECT_NAME} WIN32 ${SRC} icon/res.rc)
    # copy .dll files
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPS_DIR}/bin $<TARGET_FILE_DIR:${PROJECT_NAME}>/)
else ()
    add_executable(${PROJECT_NAME} ${SRC})
endif ()

# local includes
target_include_directories(${PROJECT_NAME} PRIVATE "src" "src/data_struct" "src/game" "src/init" "src/menus" "src/network" "src/settings" "src/utils")

# dependencies libraries
if (WIN32)
    target_link_libraries(${PROJECT_NAME} ${SDL2MAIN_LIBRARY} ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${SDL2_MIXER_LIBRARY} ${SDL2_GFX_LIBRARY} ${SDL2_NET_LIBRARY} ${DEPS_DIR}/Curl/x64/lib/libcurl.lib)
else ()
    target_link_libraries(${PROJECT_NAME} ${SDL2MAIN_LIBRARY} ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${SDL2_MIXER_LIBRARY} ${SDL2_GFX_LIBRARY} ${SDL2_NET_LIBRARY} ${CURL_LIBRARIES})
endif ()

# Other
if (MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS /SUBSYSTEM:WINDOWS)
else ()
    target_link_libraries(${PROJECT_NAME} m)
endif ()
