cmake_minimum_required(VERSION 3.0)
project(space-war)

aux_source_directory(src SRC)

add_compile_options(-Wall -Wextra -Werror -pedantic -std=c99 -O2)
add_link_options(-lSDL2 -lSDL2_image -lSDL2_gfx)

add_executable(${PROJECT_NAME} ${SRC})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data $<TARGET_FILE_DIR:${PROJECT_NAME}>/data)
