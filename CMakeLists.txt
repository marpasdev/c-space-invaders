cmake_minimum_required(VERSION 3.12)

project(SpaceInvaders C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
set(CMAKE_C_STANDARD 11)

find_package(SDL2 REQUIRED)

add_executable(SpaceInvaders main.c player.c enemy.c utils.c projectile.c dynarray.c)

target_include_directories(SpaceInvaders PRIVATE ${SDL2_INCLUDE_DIRS})

target_link_libraries(SpaceInvaders SDL2 SDL2_image SDL2_ttf)

file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})