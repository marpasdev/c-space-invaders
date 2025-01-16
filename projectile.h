#pragma once

#include "dynarray.h"
#include "player.h"

#include <SDL2/SDL.h>

typedef struct {
    float x;
    float y;
} Projectile;

void drawProjectiles(SDL_Renderer* renderer, DynArray* projectiles, SDL_Texture* texture, int scale, double deltaTime);

void checkCollisionWithEnemies(DynArray* projectiles, DynArray* enemies, SDL_Texture* projectileTexture, int scale,
    SDL_Texture* enemySmallTexture, SDL_Texture* enemyMediumTexture, SDL_Texture* enemyLargeTexture, int* score);

void moveEnemyProjectiles(DynArray* projectiles, float speed, Uint32 deltaTime);

void removeOffscreenProjectiles(DynArray* projectiles, int maxY);

void CheckCollisionWithPlayer(Player* player, DynArray* projectiles, int scale, SDL_Texture* projectileTexture);

void drawEnemyProjectiles(SDL_Renderer* renderer, DynArray* projectiles, SDL_Texture* projectileTexture, int scale);