#pragma once

#include <string.h>

typedef struct {
    void** items;
    size_t size;
    size_t capacity;
} DynArray;

void dynarray_create(DynArray* dynarray, size_t initial_capacity);

void dynarray_add(DynArray* dynarray, void* item);

void dynarray_remove(DynArray* dynarray, void* item);

void dynarray_destroy(DynArray* dynarray);