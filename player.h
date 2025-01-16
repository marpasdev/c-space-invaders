#pragma once

#include <stdbool.h>
#include <SDL2/SDL.h>

typedef struct {
    float x;
    float y;
    int w;
    int h;
    int lives;
    float speed;
} Player;

void player_move(Player* player, bool isMovingLeft, bool isMovingRight, Uint32 deltaTime);