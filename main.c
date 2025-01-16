#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <assert.h>
#include <stdbool.h>
#include <time.h>
#include "dynarray.h"
#include "player.h"
#include "utils.h"
#include "enemy.h"
#include "projectile.h"

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 800

void drawText(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, SDL_Rect location, const char* text)
{
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_RenderCopy(renderer, texture, NULL, &location);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void loadHiscore(const char* path, int* hiscore) {
    FILE* file = fopen(path, "rb");
    
    if (file != NULL) {
        if (fread(hiscore, sizeof(int), 1, file) != 1) {
            *hiscore = 0;
        }
        fclose(file);
    }
}

void saveHiscore(const char* path, int *hiscore) {
    FILE* file = fopen(path, "wb");

    fwrite(hiscore, sizeof(int), 1, file);

    fclose(file);
}

int main(int argc, char* argv[]) {

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    SDL_Window* window = SDL_CreateWindow(
        "Space Invaders Clone",
        100,
        100,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window, -1,
        SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED
    );

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    TTF_Font* font = TTF_OpenFont("assets/FFFFORWA.TTF", 20);
    SDL_Color fontColor = { .r = 255, .g = 255, .b = 255, .a = 255 };

    SDL_Event event;
    
    Uint32 lastTickTime = 0;
    Uint32 deltaTime = 0;

    double shootCooldownMax = 300;
    double shootCooldown = shootCooldownMax;

    double enemyStepCooldownMax = 1000;
    double enemyStepCooldown = enemyStepCooldownMax;
    bool isEnemySecondStep = false;

    double enemyShootCooldownMax = 1000;
    double enemyShootCooldown = enemyShootCooldownMax;

    int timeSeed = (int)time(NULL);
    srand(timeSeed);

    int scale = 4;

    SDL_Texture* playerTexture = IMG_LoadTexture(renderer, "assets/player.png");
    assert(playerTexture);
    SDL_Texture* projectileTexture = IMG_LoadTexture(renderer, "assets/player_projectile.png");
    assert(projectileTexture);
    SDL_Texture* enemySmallTexture = IMG_LoadTexture(renderer, "assets/enemy_small.png");
    assert(enemySmallTexture);
    SDL_Texture* enemyMediumTexture = IMG_LoadTexture(renderer, "assets/enemy_medium.png");
    assert(enemyMediumTexture);
    SDL_Texture* enemyLargeTexture = IMG_LoadTexture(renderer, "assets/enemy_large.png");
    assert(enemyLargeTexture);
    SDL_Texture* enemySmallTexture2 = IMG_LoadTexture(renderer, "assets/enemy_small_2.png");
    assert(enemySmallTexture2);
    SDL_Texture* enemyMediumTexture2 = IMG_LoadTexture(renderer, "assets/enemy_medium_2.png");
    assert(enemyMediumTexture2);
    SDL_Texture* enemyLargeTexture2 = IMG_LoadTexture(renderer, "assets/enemy_large_2.png");
    assert(enemyLargeTexture2);

    int score = 0;
    int hiscore = 0;
    loadHiscore("hiscore.bin", &hiscore);
    bool isGameOver = false;
    int round = 1;
    bool isStartMenu = true;

    Player player = { .lives = 3, .x = WINDOW_WIDTH / 2, .y = 700, .speed = 0.7f };
    SDL_QueryTexture(playerTexture, NULL, NULL, &player.w, &player.h);
    bool isPlayerMovingLeft = false;
    bool isPlayerMovingRight = false;

    DynArray enemies;
    dynarray_create(&enemies, 1);
    createRaid(&enemies, scale);
    int enemyDirX = 1;
    float enemySpeed = 0.3f;

    DynArray enemyProjectiles;
    dynarray_create(&enemyProjectiles, 1);

    DynArray projectiles;
    dynarray_create(&projectiles, 1);

    int running = 1;
    while (running) {
        Uint32 tickTime = SDL_GetTicks();
        deltaTime = tickTime - lastTickTime;
        lastTickTime = tickTime; 

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            if (event.type == SDL_KEYDOWN) {
                SDL_KeyCode code = event.key.keysym.sym;

                if (code == SDLK_LEFT) {
                    isPlayerMovingLeft = true;
                }
                if (code == SDLK_RIGHT) {
                    isPlayerMovingRight = true;
                }
                // strileni projektilu
                if (code == SDLK_SPACE && shootCooldown <= 0) {
                    Projectile* projectile = (Projectile*)malloc(sizeof(Projectile));
                    projectile->x = player.x + 6 * scale;
                    projectile->y = player.y - 4 * scale;
                    dynarray_add(&projectiles, (void*)projectile);
                    shootCooldown = shootCooldownMax;
                }
            }
            if (event.type == SDL_KEYUP) {
                   SDL_KeyCode code = event.key.keysym.sym;

                if (code == SDLK_LEFT) {
                    isPlayerMovingLeft = false;
                }
                if (code == SDLK_RIGHT) {
                    isPlayerMovingRight = false;
                }
                // resetovani hry
                if (code == SDLK_RETURN) {
                    if (isGameOver) {
                        isGameOver = false;
                        player.x = WINDOW_WIDTH / 2;
                        dynarray_destroy(&enemies);
                        dynarray_create(&enemies, 1);
                        createRaid(&enemies, scale);
                        enemySpeed = 0.3f;
                        enemyStepCooldownMax = 1000;
                        player.lives = 3;
                        saveHiscore("hiscore.bin", &hiscore);
                        score = 0;
                    } else if (isStartMenu) {
                        isStartMenu = false;
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (isStartMenu) {
            SDL_Rect rect1 = { .x = WINDOW_WIDTH / 2 - 400, .y = WINDOW_HEIGHT / 4, .w = 800, .h = 100};
            const char* text1 = "SPACE INVADERS";
            drawText(renderer, font, fontColor, rect1, text1);

            SDL_Rect rect2 = { .x = WINDOW_WIDTH / 2 - 200, .y = WINDOW_HEIGHT / 2, .w = 400, .h = 50};
            char buffer[50];
            sprintf(buffer, "HISCORE: %d", hiscore);
            drawText(renderer, font, fontColor, rect2, buffer);

            SDL_Rect rect3 = { .x = WINDOW_WIDTH / 2 - 300, .y = WINDOW_HEIGHT / 2 + 150, .w = 600, .h = 50};
            const char* text3 = "PRESS ENTER TO PLAY";
            drawText(renderer, font, fontColor, rect3, text3);
        } else if (!isGameOver) {

            // nove kolo
            if (enemies.size == 0) {
                ++round;
                enemyStepCooldownMax = 1000;
                enemySpeed = 0.3f + round * 0.01f;
                dynarray_destroy(&enemies);
                dynarray_create(&enemies, 1);
                createRaid(&enemies, scale);
            }

            if (shootCooldown > 0) {
                shootCooldown -= deltaTime;
            }

            if (enemyShootCooldown > 0) {
                enemyShootCooldown -= deltaTime;
            }

            // vykreslovani aktualnich zivotu
            if (player.lives >= 1) {
                SDL_Rect rect = { .x = WINDOW_WIDTH - player.w * scale - scale,
                            .y = scale, .w = player.w * scale, .h = player.h * scale };
                SDL_RenderCopy(renderer, playerTexture, NULL, &rect);
            }
            if (player.lives >= 2) {
                SDL_Rect rect = { .x = WINDOW_WIDTH - player.w * scale * 2 - scale * 2,
                            .y = scale, .w = player.w * scale, .h = player.h * scale };
            SDL_RenderCopy(renderer, playerTexture, NULL, &rect);
            }
            if (player.lives == 3) {
                SDL_Rect rect = { .x = WINDOW_WIDTH - player.w * scale * 3 - scale * 3,
                            .y = scale, .w = player.w * scale, .h = player.h * scale };
                SDL_RenderCopy(renderer, playerTexture, NULL, &rect);
            }

            // vypis skore behem hry
            char buffer[50];
            sprintf(buffer, "SCORE: %d", score);
            SDL_Rect scoreRect = { .x = 25, .y = 10, .w = 200, .h = 25 };
            drawText(renderer, font, fontColor, scoreRect, buffer);
            sprintf(buffer, "HISCORE: %d", hiscore);
            scoreRect.x = WINDOW_WIDTH / 2 - scoreRect.w / 2;
            drawText(renderer, font, fontColor, scoreRect, buffer);

            // spodni hranice
            SDL_Rect bottomLine = { .x = 0, .y = 750, .w = WINDOW_WIDTH, .h = scale };
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_RenderFillRect(renderer, &bottomLine);

            SDL_Rect playerRect = { .x = (int)player.x, .y = (int)player.y, .w = player.w * scale, .h = player.h * scale };

            // udrzeni hrace v mezich obrazovky
            clamp(&player.x, 0, WINDOW_WIDTH - playerRect.w);
            playerRect.x = (int)player.x;

            // vykresleni a pohyb hrace
            SDL_RenderCopy(renderer, playerTexture, NULL, &playerRect);
            player_move(&player, isPlayerMovingLeft, isPlayerMovingRight, deltaTime);

            // nepratele strili
            if (enemyShootCooldown <= 0) {
                enemiesShoot(&enemies, &enemyProjectiles, scale);
                enemyShootCooldown = enemyShootCooldownMax;
            }

            moveEnemies(&enemies, &isEnemySecondStep, &enemyStepCooldown, &enemyStepCooldownMax,
            deltaTime, &enemyDirX, &enemySpeed, WINDOW_WIDTH, scale);

            drawEnemies(renderer, &enemies, enemySmallTexture, enemySmallTexture2,
            enemyMediumTexture, enemyMediumTexture2, enemyLargeTexture, enemyLargeTexture2,
            scale, isEnemySecondStep);

            drawProjectiles(renderer, &projectiles, projectileTexture, scale, deltaTime);

            // kolize raket a nepratel
            checkCollisionWithEnemies(&projectiles, &enemies, projectileTexture, scale,
            enemySmallTexture, enemyMediumTexture, enemyLargeTexture, &score);

            // nepratelske rakety
            moveEnemyProjectiles(&enemyProjectiles, 0.3f, deltaTime);
            removeOffscreenProjectiles(&enemyProjectiles, WINDOW_HEIGHT + scale * 4);
            drawEnemyProjectiles(renderer, &enemyProjectiles, projectileTexture, scale);
            CheckCollisionWithPlayer(&player, &enemyProjectiles, scale, projectileTexture);  

            // nepratele dosahli spodni hranice
            if (enemies.size != 0 && haveEnemiesInvaded(&enemies, bottomLine.y, scale) || player.lives <= 0) {
                isGameOver = true;
            }

            if (score > hiscore) {
                hiscore = score;
            }

        } else {
            // gameover text
            SDL_Rect rect1 = { .x = WINDOW_WIDTH / 2 - 200, .y = WINDOW_HEIGHT / 4, .w = 400, .h = 100};
            const char* text1 = "GAME OVER";
            drawText(renderer, font, fontColor, rect1, text1);

            char buffer[50];
            sprintf(buffer, "SCORE: %d", score);

            SDL_Rect rect2 = { .x = WINDOW_WIDTH / 2 - 125, .y = WINDOW_HEIGHT / 2 - 20 + 75, .w = 250, .h = 40};
            drawText(renderer, font, fontColor, rect2, buffer);

            sprintf(buffer, "HISCORE: %d", hiscore);
            SDL_Rect rect3 = { .x = WINDOW_WIDTH / 2 - 125, .y = WINDOW_HEIGHT / 2 - 20 + 125, .w = 250, .h = 40};
            drawText(renderer, font, fontColor, rect3, buffer);

            SDL_Rect rect4 = { .x = WINDOW_WIDTH / 2 - 300, .y = WINDOW_HEIGHT / 2 - 20 + 250, .w = 600, .h = 50};
            const char* text2 = "PRESS ENTER TO PLAY AGAIN";
            drawText(renderer, font, fontColor, rect4, text2);
        }

        SDL_RenderPresent(renderer);
    }

    saveHiscore("hiscore.bin", &hiscore);

    dynarray_destroy(&enemies);
    dynarray_destroy(&projectiles);   

    SDL_DestroyTexture(playerTexture);
    SDL_DestroyTexture(projectileTexture);
    SDL_DestroyTexture(enemySmallTexture);
    SDL_DestroyTexture(enemyMediumTexture);
    SDL_DestroyTexture(enemyLargeTexture);
    SDL_DestroyTexture(enemySmallTexture2);
    SDL_DestroyTexture(enemyMediumTexture2);
    SDL_DestroyTexture(enemyLargeTexture2);

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}