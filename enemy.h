#pragma once

#include "dynarray.h"

#include <SDL2/SDL.h>
#include <stdbool.h>

typedef enum {
    SMALL = 40,
    MEDIUM = 20,
    LARGE = 10
} EnemyType;

typedef struct {
    float x;
    float y;
    EnemyType type;
} Enemy;

void createRaid(DynArray* enemies, int scale);

bool haveEnemiesInvaded(DynArray* enemies, int bottomLineY, int scale);

void moveEnemies(DynArray* enemies,  bool* isSecondStep, double* stepCooldown, double* stepCooldownMax, Uint32 deltaTime, 
int* dirX, float* speed, int screenWidth, int scale);

void drawEnemies(SDL_Renderer* renderer, DynArray* enemies, 
SDL_Texture* enemySmallTexture, SDL_Texture* enemySmallTexture2, SDL_Texture* enemyMediumTexture,
SDL_Texture* enemyMediumTexture2, SDL_Texture* enemyLargeTexture, SDL_Texture* enemyLargeTexture2,
int scale, bool useSecondTexture);

SDL_Rect getEnemyRect(Enemy enemy, SDL_Texture* enemySmallTexture, SDL_Texture* enemyMediumTexture, SDL_Texture* enemyLargeTexture);

void enemiesShoot(DynArray* enemies, DynArray* projectiles, int scale);