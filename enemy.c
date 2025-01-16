#include "enemy.h"
#include "dynarray.h"
#include "projectile.h"

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <time.h>

void createRaid(DynArray* enemies, int scale) {
    for (size_t i = 0; i < 5; ++i) {
        for (size_t j = 0; j < 11; ++j) {
            Enemy* enemy = (Enemy*)malloc(sizeof(Enemy));
            enemy->x = 100 + 15 * scale * j;
            enemy-> y = 150 + 15 * scale * i;
            if (i == 0) {
                enemy->type = SMALL;
            } else if (i == 1 || i == 2) {
                enemy->type = MEDIUM;
            } else if (i == 3 || i == 4) {
                enemy->type = LARGE;
            }
            dynarray_add(enemies, (void*)enemy);
        }
    }
}

bool haveEnemiesInvaded(DynArray* enemies, int bottomLineY, int scale) {
    Enemy* enemy = (Enemy*)enemies->items[enemies->size - 1];
    
    if (enemy->y + 10 * scale >= bottomLineY) {
        return true;
    }
    return false;
}

void moveEnemies(DynArray* enemies,  bool* isSecondStep, double* stepCooldown, double* stepCooldownMax, Uint32 deltaTime,
                int* dirX, float* speed, int screenWidth, int scale) {
    bool changeDir = false;
    
    if (*stepCooldown <= 0) {
            *isSecondStep = !(*isSecondStep);

            for (size_t i = 0; i < enemies->size; ++i) {
                Enemy* enemy = (Enemy*)enemies->items[i];
                enemy->x += deltaTime * *speed * *dirX;
                if (enemy->x + deltaTime * *speed * *dirX <= 0) {
                    changeDir = true;
                } else if (enemy->x + deltaTime * *speed > (screenWidth) - scale * 10) {
                    changeDir = true;
                }
            }

            if (changeDir) {
                *dirX = -*dirX;
                for (size_t i = 0; i < enemies->size; ++i) {
                    Enemy* enemy = (Enemy*)enemies->items[i];
                    enemy->y += scale * 5;
                }
                *stepCooldownMax /= 2;
            }

            *stepCooldown = *stepCooldownMax;
        } else {
            *stepCooldown -= deltaTime;
        }
}

void drawEnemies(SDL_Renderer* renderer, DynArray* enemies, 
SDL_Texture* enemySmallTexture, SDL_Texture* enemySmallTexture2, SDL_Texture* enemyMediumTexture,
SDL_Texture* enemyMediumTexture2, SDL_Texture* enemyLargeTexture, SDL_Texture* enemyLargeTexture2,
int scale, bool useSecondTexture) {
    for (size_t i = 0; i < enemies->size; ++i) {
            Enemy* enemy = (Enemy*)enemies->items[i];

            SDL_Rect rect = { .x = (int)enemy->x, .y = (int)enemy->y, .w = 0, .h = 0 };
            switch (enemy->type) {
                case SMALL:
                    SDL_QueryTexture(enemySmallTexture, NULL, NULL, &rect.w, &rect.h);
                    rect.w *= scale;
                    rect.h *= scale;
                    if (useSecondTexture) {
                        SDL_RenderCopy(renderer, enemySmallTexture2, NULL, &rect );
                    } else {
                        SDL_RenderCopy(renderer, enemySmallTexture, NULL, &rect );
                    }
                    break;
                case MEDIUM:
                    SDL_QueryTexture(enemyMediumTexture, NULL, NULL, &rect.w, &rect.h);
                    rect.w *= scale;
                    rect.h *= scale;
                    if (useSecondTexture) {
                        SDL_RenderCopy(renderer, enemyMediumTexture2, NULL, &rect );
                    } else {
                        SDL_RenderCopy(renderer, enemyMediumTexture, NULL, &rect );
                    }
                    break;
                case LARGE:
                    SDL_QueryTexture(enemyLargeTexture, NULL, NULL, &rect.w, &rect.h);
                    rect.w *= scale;
                    rect.h *= scale;
                    if (useSecondTexture) {
                        SDL_RenderCopy(renderer, enemyLargeTexture2, NULL, &rect );
                    } else {
                        SDL_RenderCopy(renderer, enemyLargeTexture, NULL, &rect );
                    }
                    break;
            }
        }
}

SDL_Rect getEnemyRect(Enemy enemy, SDL_Texture* enemySmallTexture, SDL_Texture* enemyMediumTexture, SDL_Texture* enemyLargeTexture) {
    SDL_Rect enemyRect = { .x = enemy.x, .y = enemy.y, .w = 0, .h = 0 };

    // nacteni rozmeru textury nepritele
    switch (enemy.type) {
        case SMALL:
            SDL_QueryTexture(enemySmallTexture, NULL, NULL, &enemyRect.w, &enemyRect.h);
        break;
        case MEDIUM:
            SDL_QueryTexture(enemyMediumTexture, NULL, NULL, &enemyRect.w, &enemyRect.h);
        break;
        case LARGE:
            SDL_QueryTexture(enemyLargeTexture, NULL, NULL, &enemyRect.w, &enemyRect.h);
        break;
    }

    return enemyRect;
}

void enemiesShoot(DynArray* enemies, DynArray* projectiles, int scale) {
    for (size_t i = 0; i < enemies->size; ++i) {
        Enemy* enemy = (Enemy*)enemies->items[i];

        if (rand() % 100 < 1) {
            Projectile* projectile = (Projectile*)malloc(sizeof(Projectile));
            projectile->x = enemy->x + scale * 5;
            projectile->y = enemy->y + scale * 5;
            dynarray_add(projectiles, (void*)projectile);
        }
    }
}