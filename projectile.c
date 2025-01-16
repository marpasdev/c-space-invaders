#include "projectile.h"

#include "dynarray.h"
#include "enemy.h"
#include "utils.h"
#include "player.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

void drawProjectiles(SDL_Renderer* renderer, DynArray* projectiles, SDL_Texture* texture, int scale, double deltaTime) {
    for (size_t i = 0; i < projectiles->size; ++i) {

        Projectile* projectile = (Projectile*)projectiles->items[i];

        int projectileHeight = 0;
        SDL_QueryTexture(texture, NULL, NULL, NULL, &projectileHeight);

        if (projectile->y <= -projectileHeight * scale) {
            dynarray_remove(projectiles, (void*)projectile);
            --i;
        } else {
            SDL_Rect rect = { .x = (int)projectile->x, .y = (int)projectile->y, .w = scale, .h = 4 * scale };
            SDL_RenderCopy(renderer, texture, NULL, &rect);
            projectile->y -= deltaTime * 0.5f;
        }
    }
}

void moveEnemyProjectiles(DynArray* projectiles, float speed, Uint32 deltaTime) {
    for (size_t i = 0; i < projectiles->size; ++i) {
        Projectile* projectile = (Projectile*)projectiles->items[i];
        projectile->y += deltaTime * speed;
    }
}

void drawEnemyProjectiles(SDL_Renderer* renderer, DynArray* projectiles, SDL_Texture* projectileTexture, int scale) {
    int w = 0;
    int h = 0;
    SDL_QueryTexture(projectileTexture, NULL, NULL, &w, &h);
    for (size_t i = 0; i < projectiles->size; ++i) {
        Projectile* projectile = (Projectile*)projectiles->items[i];
        SDL_Rect rect = { .x = projectile->x, .y = projectile->y,
        .w = w * scale, .h = h * scale };

        SDL_RenderCopy(renderer, projectileTexture, NULL, &rect);
    }
}

void removeOffscreenProjectiles(DynArray* projectiles, int maxY) {
    for (size_t i = 0; i < projectiles->size; ++i) {
        Projectile* projectile = (Projectile*)projectiles->items[i];

        if (projectile->y > maxY) {
            dynarray_remove(projectiles, (void*)projectile);
        }
    }
}

void checkCollisionWithEnemies(DynArray* projectiles, DynArray* enemies, SDL_Texture* projectileTexture, int scale,
    SDL_Texture* enemySmallTexture, SDL_Texture* enemyMediumTexture, SDL_Texture* enemyLargeTexture, int* score) {
    for (size_t i = 0; i < projectiles->size; ++i) {
        Projectile* projectile = (Projectile*)projectiles->items[i];

        SDL_Rect projectileRect = { .x = projectile->x, .y = projectile->y, .w = 0, .h = 0 };
        SDL_QueryTexture(projectileTexture, NULL, NULL, &projectileRect.w, &projectileRect.h);
        projectileRect.w *= scale;
        projectileRect.h *= scale;

        for (size_t j = 0; j < enemies->size; ++j) {
            Enemy* enemy = (Enemy*)enemies->items[j];
            SDL_Rect enemyRect = getEnemyRect(*enemy, enemySmallTexture, enemyMediumTexture, enemyLargeTexture);

            enemyRect.w *= scale;
            enemyRect.h *= scale;

            if (isCollision(projectileRect, enemyRect)) {
                *score += enemy->type;
                dynarray_remove(projectiles, (void*)projectile);
                dynarray_remove(enemies, (void*)enemy);
                --i;
                break;                                           
            }
        }
    }
}

void CheckCollisionWithPlayer(Player* player, DynArray* projectiles, int scale, SDL_Texture* projectileTexture) {
    SDL_Rect playerRect = { .x = player->x, .y = player->y, .w = player->w * scale, .h = player->h * scale };

    int w = 0;
    int h = 0;
    SDL_QueryTexture(projectileTexture, NULL, NULL, &w, &h);
    for (size_t i = 0; i < projectiles->size; ++i) {
        Projectile* projectile = (Projectile*)projectiles->items[i];
        SDL_Rect projectileRect = { .x = projectile->x, .y = projectile->y, .w = w * scale, .h = h * scale };

        if (isCollision(playerRect, projectileRect)) {
            --player->lives;
            dynarray_remove(projectiles, (void*)projectile);
            --i;
        }
    }
}