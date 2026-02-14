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
    void array_dynamic_init(array* a, size_t element_size, size_t array_size)
    void array_dynamic_fini(array* a)
    void array_static_init(array* a, void* static_container, size_t element_size, size_t array_size)
    void* array_at(array* a, size_t index)
    void array_insert(array* a, void* element, size_t index)
    void array_remove(array* a, size_t index)
    void array_push_back(array* a, void* element)
    void array_pop_front(array* a)
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
    void* elements;
    size_t element_size;
    size_t count;
    size_t capacity;
    bool is_dynamic;
} array;

void array_dynamic_init(array* a, size_t element_size, size_t array_size) {
    assert(a && "Passed NULL array pointer!");
    a->is_dynamic = true;
    a->element_size = element_size;
    a->count = 0;
    a->capacity = element_size * array_size;
    a->elements = malloc(sizeof(void*) * a->capacity);
    assert(a->elements && "Malloc failed");
#ifdef ARRAY_DEBUG_LOG
    printf("Initialized dynamic array with a capacity of %ld\n", a->capacity);
#endif
}

void array_dynamic_fini(array* a) {
    assert(a && "Passed NULL array pointer!");
    assert(a->is_dynamic && "Passed static array pointer!");
    free(a->elements);
#ifdef ARRAY_DEBUG_LOG
    printf("Freed %ld elements\n", a->capacity);
#endif
}

void array_static_init(array* a, void* static_container, size_t element_size, size_t array_size) {
    assert(a && "Passed NULL array pointer!");
    a->is_dynamic = false;
    a->element_size = element_size;
    a->count = 0;
    a->capacity = element_size * array_size;
    a->elements = static_container;
#ifdef ARRAY_DEBUG_LOG
    printf("Initialized static array with a capacity of %ld\n", a->capacity);
#endif
}

void* array_at(array* a, size_t index) {
    assert(a && "Passed NULL array pointer!");
    assert(index < a->count && "Index error");
    return (char*)(a->elements) + (a->element_size * index);
}

void array_insert(array* a, void* element, size_t index) {
    assert(a && "Passed NULL array pointer!");
    assert(element && "Passed NULL element pointer!");
    assert(index <= a->count && "Index error");
    a->count += 1;
    
    // if there is not enough capacity, then double the capacity
    if (a->is_dynamic && a->count * a->element_size > a->capacity) {
        a->capacity *= 2;
        a->elements = realloc(a->elements, a->capacity);
        assert(a->elements && "Cannot realloc!");
#ifdef ARRAY_DEBUG_LOG
        printf("Increased capacity to %ld\n", a->capacity);
#endif
    }
    assert(a->count * a->element_size <= a->capacity && "Array overflow!");

    void* element_slot = array_at(a, index);

    // if needed, shift the rest of the elements to the right
    if (index + 1 < a->count) {
        void* next_element_slot = array_at(a, index + 1);
        memcpy(next_element_slot, element_slot, (a->count - (index + 1)) * a->element_size);
    }

    // copy the new element into the slot
    memcpy(element_slot, element, a->element_size);
#ifdef ARRAY_DEBUG_LOG
    printf("Added element at index %ld, count is %ld\n", index, a->count);
#endif
}

void array_remove(array* a, size_t index) {
    assert(a && "Passed NULL array pointer!");
    assert(index < a->count && "Index error");

    // if needed, shift the rest of the elements to the left
    if (index + 1 < a->count) {
        void* element_slot = array_at(a, index);
        void* next_element_slot = array_at(a, index + 1);
        memcpy(element_slot, next_element_slot, (a->count - (index + 1)) * a->element_size);
    }
    a->count -= 1;
#ifdef ARRAY_DEBUG_LOG
    printf("Removed element at index %ld, count is %ld\n", index, a->count);
#endif

    // if 1/4 of the capacity is not used, then reduce it by half
    if (a->is_dynamic && a->count > 0 && a->count * a->element_size <= a->capacity / 4) {
        a->capacity = a->count * a->element_size * 2;
        a->elements = realloc(a->elements, a->capacity);
        assert(a->elements && "Cannot realloc!");
#ifdef ARRAY_DEBUG_LOG
        printf("Decreased capacity to %ld\n", a->capacity);
#endif
    }
}

void array_push_back(array* a, void* element) {
    assert(a && "Passed NULL array pointer!");
    array_insert(a, element, a->count);
}

void array_pop_front(array* a) {
    array_remove(a, 0);
}

size_t array_size(array* a) {
    assert(a && "Passed NULL array pointer!");
    return a->count;
}

bool array_empty(array* a) {
    return (array_size(a) == 0);
}

#endif  // __ARRAY_H__