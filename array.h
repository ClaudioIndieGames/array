/*
    This library implements a dynamic and static array interface.

    Array contruction/destruction:
    (For a dynamic array)
    array a;
    array_dynamic_init(&a, sizeof(<array_type>), <initial_array_size>);
    array_dynamic_fini(&a);

    (For a static array)
    array a;
    <array_type> container[<array_size>];
    array_static_init(&a, &container, sizeof(<array_type>), sizeof(container) / sizeof(<array_type>));

    API:
    void array_dynamic_init(array* a, size_t slot_size, size_t array_size)
    void array_dynamic_fini(array* a)
    void array_static_init(array* a, void* static_container, size_t slot_size, size_t array_size)
    void* array_at(array* a, size_t index)
    void* array_insert_slot(array* a, size_t index)
    void* array_insert_copy(array* a, void* value, size_t index)
    void array_remove(array* a, size_t index)
    void* array_append_copy(array* a, void* value)
    void* array_append_slot(array* a)
    void array_pop(array* a)
    void* array_front(array* a)
    void* array_back(array* a)
    size_t array_size(array* a)
    bool array_empty(array* a)

    Claudio Raccomandato, February 15 2026
*/

#ifndef __ARRAY_H__
#define __ARRAY_H__

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#ifdef ARRAY_DEBUG_LOG
#include <stdio.h>
#endif

typedef struct {
    void* slots;
    size_t slot_size;
    size_t count;
    size_t capacity;
    bool is_dynamic;
} array;

void array_dynamic_init(array* a, size_t slot_size, size_t array_size) {
    assert(a && "Passed NULL array pointer!");
    a->is_dynamic = true;
    a->slot_size = slot_size;
    a->count = 0;
    a->capacity = slot_size * array_size;
    a->slots = malloc(sizeof(void*) * a->capacity);
    assert(a->slots && "Malloc failed");
#ifdef ARRAY_DEBUG_LOG
    printf("Initialized dynamic array with a capacity of %ld bytes\n", a->capacity);
#endif
}

void array_dynamic_fini(array* a) {
    assert(a && "Passed NULL array pointer!");
    assert(a->is_dynamic && "Passed static array pointer!");
    free(a->slots);
#ifdef ARRAY_DEBUG_LOG
    printf("Freed %ld bytes\n", a->capacity);
#endif
}

void array_static_init(array* a, void* static_container, size_t slot_size, size_t array_size) {
    assert(a && "Passed NULL array pointer!");
    a->is_dynamic = false;
    a->slot_size = slot_size;
    a->count = 0;
    a->capacity = slot_size * array_size;
    a->slots = static_container;
#ifdef ARRAY_DEBUG_LOG
    printf("Initialized static array with a capacity of %ld bytes\n", a->capacity);
#endif
}

void* array_at(array* a, size_t index) {
    assert(a && "Passed NULL array pointer!");
    assert(index < a->count && "Index error");
    return (char*)(a->slots) + (a->slot_size * index);
}

void* array_insert_slot(array* a, size_t index) {
    assert(a && "Passed NULL array pointer!");
    assert(index <= a->count && "Index error");
    a->count += 1;
    
    // if there is not enough capacity, then double the capacity
    if (a->is_dynamic && a->count * a->slot_size > a->capacity) {
        a->capacity *= 2;
        a->slots = realloc(a->slots, a->capacity);
        assert(a->slots && "Cannot realloc!");
#ifdef ARRAY_DEBUG_LOG
        printf("Increased capacity to %ld bytes\n", a->capacity);
#endif
    }
    assert(a->count * a->slot_size <= a->capacity && "Array overflow!");

    void* slot = array_at(a, index);

    // if needed, shift the rest of the slots to the right
    if (index + 1 < a->count) {
        void* next_slot = array_at(a, index + 1);
        memcpy(next_slot, slot, (a->count - (index + 1)) * a->slot_size);
    }

#ifdef ARRAY_DEBUG_LOG
    printf("Inserted slot at index %ld, count is %ld\n", index, a->count);
#endif

    return slot;
}

void* array_insert_copy(array* a, void* value, size_t index) {
    assert(value && "Passed NULL value pointer!");
    void* slot = array_insert_slot(a, index);

    // copy the value into the new slot
    memcpy(slot, value, a->slot_size);
#ifdef ARRAY_DEBUG_LOG
    printf("Stored copy at index %ld\n", index);
#endif

    return slot;
}

void array_remove(array* a, size_t index) {
    assert(a && "Passed NULL array pointer!");
    assert(index < a->count && "Index error");

    // if needed, shift the rest of the slots to the left
    if (index + 1 < a->count) {
        void* slot = array_at(a, index);
        void* next_slot = array_at(a, index + 1);
        memcpy(slot, next_slot, (a->count - (index + 1)) * a->slot_size);
    }
    a->count -= 1;
#ifdef ARRAY_DEBUG_LOG
    printf("Removed slot at index %ld, count is %ld\n", index, a->count);
#endif

    // if 1/4 of the capacity is not used, then reduce it by half
    if (a->is_dynamic && a->count > 0 && a->count * a->slot_size <= a->capacity / 4) {
        a->capacity = a->count * a->slot_size * 2;
        a->slots = realloc(a->slots, a->capacity);
        assert(a->slots && "Cannot realloc!");
#ifdef ARRAY_DEBUG_LOG
        printf("Decreased capacity to %ld bytes\n", a->capacity);
#endif
    }
}

void* array_append_copy(array* a, void* value) {
    assert(a && "Passed NULL array pointer!");
    return array_insert_copy(a, value, a->count);
}

void* array_append_slot(array* a) {
    assert(a && "Passed NULL array pointer!");
    return array_insert_slot(a, a->count);
}

void array_pop(array* a) {
    array_remove(a, 0);
}

void* array_front(array* a) {
    return array_at(a, 0);
}

void* array_back(array* a) {
    assert(a && "Passed NULL array pointer!");
    assert(a->count > 0 && "Index error");
    return array_at(a, a->count - 1);
}

size_t array_size(array* a) {
    assert(a && "Passed NULL array pointer!");
    return a->count;
}

bool array_empty(array* a) {
    return (array_size(a) == 0);
}

#endif  // __ARRAY_H__