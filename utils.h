#pragma once

#include <SDL2/SDL.h>
#include <stdbool.h>

bool isCollision(SDL_Rect a, SDL_Rect b);

void clamp(float* value, float min, float max);