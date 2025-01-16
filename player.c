#include "player.h"

#include <stdbool.h>
#include <SDL2/SDL.h>

void player_move(Player* player, bool isMovingLeft, bool isMovingRight, Uint32 deltaTime) {
    if (isMovingLeft) {
            player->x -= deltaTime * player->speed;
        }
        if (isMovingRight) {
            player->x += deltaTime * player->speed;
        }
}