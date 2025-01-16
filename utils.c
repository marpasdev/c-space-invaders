#include "utils.h"

#include <SDL2/SDL.h>
#include <stdbool.h>

bool isCollision(SDL_Rect a, SDL_Rect b) {
    if (a.x < b.x + b.w && a.x + a.w > b.x &&
        a.y < b.y + b.h && a.y + a.h > b.y) {
        return true;
    }
    return false;
}

void clamp(float* value, float min, float max) {
    if (*value < min) {
            *value = min;
        } else if (*value > max) {
            *value = max;
        }
}