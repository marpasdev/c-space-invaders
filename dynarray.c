#include "dynarray.h"

#include <stdlib.h>
#include <string.h>

void dynarray_create(DynArray* dynarray, size_t initial_capacity) {
    dynarray->capacity = initial_capacity;
    dynarray->size = 0;
    dynarray->items = (void**)malloc(sizeof(void*) * dynarray->capacity);
    memset(dynarray->items, 0, sizeof(void*) * dynarray->capacity);
}

void dynarray_add(DynArray* dynarray, void* item) {
    if (dynarray->size == dynarray->capacity) {
        size_t new_capacity = dynarray->capacity * 2;
        void** memory = (void**)malloc(sizeof(void*) * new_capacity);
        memcpy(memory, dynarray->items, dynarray->size * sizeof(void*));
        free(dynarray->items);

        dynarray->items = memory;
        dynarray->capacity = new_capacity;
    }

    dynarray->items[dynarray->size] = item;
    ++dynarray->size;
}

void dynarray_remove(DynArray* dynarray, void* item) {
    for (size_t i = 0; i < dynarray->size; ++i) {
        if (dynarray->items[i] == item) {
            free(item);

            for (size_t j = i + 1; j < dynarray->size; ++j) {
                dynarray->items[j - 1] = dynarray->items[j];
            }

            --dynarray->size;
            --i;
        }
    }
}

void dynarray_destroy(DynArray* dynarray) {
    for (size_t i = 0; i < dynarray->size; ++i) {
        free(dynarray->items[i]);
    }
    free(dynarray->items);
}