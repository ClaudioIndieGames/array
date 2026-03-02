/*
    # array.h
    ## Description
    This header-only library implements a universal interface for *dynamic* and *static* arrays.

    ## How to use it
    Since it's a header-only library, all you need to do is use `#include "array.h"` in your project files.
    Only remember to define `ARRAY_IMPLEMENTATION`, in exactly ***one*** translation unit (e.g., main.c) before the inclusion:
    ```c
    #define ARRAY_IMPLEMENTATION
    #include "array.h"
    ```

    ### Header and container
    The library uses a header (the "array" type, including all the array metadata) and a container (the actual data).
    Header and container can be provided by the user (static implementation) or created by the library (dynamic implementation).
    There are four possible array combinations:

    1. dynamic array (header and container created by the library)
    ```c
    // array* array_create_dynamic(size_t slot_size, size_t initial_container_size)
    array* a = array_create_dynamic(sizeof(int), 10);
    array_destroy(a);
    ```

    3. semi-dynamic array (header provided by the user and container created by the library) **[MOST COMMON]**
    ```c
    // array* array_create_semi_dynamic(array* a, size_t slot_size, size_t initial_container_size)
    array header;
    array* a = array_create_semi_dynamic(&header, sizeof(int), 10);
    // or array* a = array_create_semi_dynamic(&(array){0}, sizeof(int), 10);
    array_destroy(a);
    ```

    4. semi-static array (header created by the library and container provided by the user)
    ```c
    // array* array_create_semi_static(void* static_container, size_t slot_size, size_t container_size)
    int container[10];
    array* a = array_create_semi_static(&container, sizeof(int), 10);
    // or array* a = array_create_semi_static(&(int[10]){0}, sizeof(int), 10);
    array_destroy(a);
    ```

    5. static array (header and container provided by the user)
    ```c
    // array* array_create_static(array* a, void* static_container, size_t slot_size, size_t container_size)
    array header;
    int container[int];
    array* a = array_create_static(&header, &container, sizeof(int), 10);
    // or array* a = array_create_semi_static(&(array){0}, &(int[10]){0}, sizeof(int), 10);
    array_destroy(a);  // optional
    ```

    ## API
    ```c
    array* array_create(array_dynamicity_e dynamicity, size_t slot_size, size_t container_size, array* a, void* static_container)
    array* array_create_dynamic(size_t slot_size, size_t initial_container_size)
    array* array_create_semi_dynamic(array* a, size_t slot_size, size_t initial_container_size)
    array* array_create_semi_static(void* static_container, size_t slot_size, size_t container_size)
    array* array_create_static(array* a, void* static_container, size_t slot_size, size_t container_size)
    void array_destroy(array* a)
    void* array_at(array* a, size_t index)
    void* array_insert_slot(array* a, size_t index)
    void* array_insert_copy(array* a, void* value, size_t index)
    void array_remove(array* a, size_t index)
    void array_clear(array* a)
    void* array_append_copy(array* a, void* value)
    void* array_append_slot(array* a)
    void array_pop(array* a)
    void* array_front(array* a)
    void* array_back(array* a)
    size_t array_size(array* a)
    char array_empty(array* a)
    void array_sort(array* a, int (*compare_func)(const void*, const void*))
    void* array_find(array* a, const void* value, int (*compare_func)(const void*, const void*))
    char array_binary_search(array* a, const void* value, int (*compare_func)(const void*, const void*), size_t* pos)
    ```

    Claudio Raccomandato, February 15 2026
*/

#ifndef __ARRAY_H__
#define __ARRAY_H__

#include <stddef.h>

typedef enum {
    ARRAY_DYNAMIC,
    ARRAY_SEMI_DYNAMIC,
    ARRAY_SEMI_STATIC,
    ARRAY_STATIC
} array_dynamicity_e ; 

typedef struct {
    void* slots;
    size_t slot_size;
    size_t count;
    size_t capacity;
    array_dynamicity_e dynamicity;
} array;

array* array_create(array_dynamicity_e dynamicity, size_t slot_size, size_t container_size, array* a, void* static_container);
array* array_create_dynamic(size_t slot_size, size_t initial_container_size);
array* array_create_semi_dynamic(array* a, size_t slot_size, size_t initial_container_size);
array* array_create_semi_static(void* static_container, size_t slot_size, size_t container_size);
array* array_create_static(array* a, void* static_container, size_t slot_size, size_t container_size);
void array_destroy(array* a);
void* array_at(array* a, size_t index);
void* array_insert_slot(array* a, size_t index);
void* array_insert_copy(array* a, void* value, size_t index);
void array_remove(array* a, size_t index);
void array_clear(array* a);
void* array_append_copy(array* a, void* value);
void* array_append_slot(array* a);
void array_pop(array* a);
void* array_front(array* a);
void* array_back(array* a);
size_t array_size(array* a);
char array_empty(array* a);
void array_sort(array* a, int (*compare_func)(const void*, const void*));
void* array_find(array* a, const void* value, int (*compare_func)(const void*, const void*));
char array_binary_search(array* a, const void* value, int (*compare_func)(const void*, const void*), size_t* pos);

#ifdef ARRAY_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef ARRAY_DEBUG_LOG
#include <stdio.h>
#endif

array* array_create(array_dynamicity_e dynamicity, size_t slot_size, size_t container_size, array* a, void* static_container) {
    assert(slot_size > 0 && "Slot must be larger than 0");
    assert(container_size > 0 && "Container must be larger than 0");
    switch (dynamicity)
    {
    case ARRAY_DYNAMIC: {
        a = malloc(sizeof(array));
        assert(a && "Malloc failed");
        a->slots = malloc(sizeof(void*) * slot_size * container_size);
        assert(a->slots && "Malloc failed");
    } break;
    case ARRAY_SEMI_DYNAMIC: {
        assert(a && "Passed NULL array");
        a->slots = malloc(sizeof(void*) * slot_size * container_size);
        assert(a->slots && "Malloc failed");
    } break;
    case ARRAY_SEMI_STATIC: {
        a = malloc(sizeof(array));
        assert(a && "Malloc failed");
        assert(static_container && "Passed NULL container");
        a->slots = static_container;
    } break;
    case ARRAY_STATIC: {
        assert(a && "Passed NULL array");
        assert(static_container && "Passed NULL container");
        a->slots = static_container;
    } break;
    default:
        assert(0 && "Wrong dynamicity");
    }
    a->slot_size = slot_size;
    a->count = 0;
    a->capacity = slot_size * container_size;
    a->dynamicity = dynamicity;

    return a;
}

array* array_create_dynamic(size_t slot_size, size_t initial_container_size) {
    array* a = array_create(ARRAY_DYNAMIC, slot_size, initial_container_size, NULL, NULL);
#ifdef ARRAY_DEBUG_LOG
    printf("Initialized dynamic array with a capacity of %ld bytes\n", a->capacity);
#endif
    return a;
}

array* array_create_semi_dynamic(array* a, size_t slot_size, size_t initial_container_size) {
    array_create(ARRAY_SEMI_DYNAMIC, slot_size, initial_container_size, a, NULL);
#ifdef ARRAY_DEBUG_LOG
    printf("Initialized semi-dynamic array with a capacity of %ld bytes\n", a->capacity);
#endif
    return a;
}

array* array_create_semi_static(void* static_container, size_t slot_size, size_t container_size) {
    array* a = array_create(ARRAY_SEMI_STATIC, slot_size, container_size, NULL, static_container);
#ifdef ARRAY_DEBUG_LOG
    printf("Initialized semi-static array with a capacity of %ld bytes\n", a->capacity);
#endif
    return a;
}

array* array_create_static(array* a, void* static_container, size_t slot_size, size_t container_size) {
    array_create(ARRAY_STATIC, slot_size, container_size, a, static_container);
#ifdef ARRAY_DEBUG_LOG
    printf("Initialized static array with a capacity of %ld bytes\n", a->capacity);
#endif
    return a;
}

void array_destroy(array* a) {
    assert(a && "Passed NULL array pointer!");
    switch (a->dynamicity)
    {
    case ARRAY_DYNAMIC: {
        assert(a->slots && "Free failed");
        free(a->slots);
#ifdef ARRAY_DEBUG_LOG
        printf("Freed %ld bytes\n", a->capacity);
#endif
        assert(a && "Free failed");
        free(a);
#ifdef ARRAY_DEBUG_LOG
        printf("Freed %ld bytes\n", sizeof(array));
#endif
    } break;
    case ARRAY_SEMI_DYNAMIC: {
        assert(a->slots && "Free failed");
        free(a->slots);
#ifdef ARRAY_DEBUG_LOG
        printf("Freed %ld bytes\n", a->capacity);
#endif
    } break;
    case ARRAY_SEMI_STATIC: {
        assert(a && "Free failed");
        free(a);
#ifdef ARRAY_DEBUG_LOG
        printf("Freed %ld bytes\n", sizeof(array));
#endif
    } break;
    case ARRAY_STATIC: {
        // nop
    } break;
    default:
        assert(0 && "Wrong dynamicity");
    }
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
    if (a->dynamicity == ARRAY_DYNAMIC || a->dynamicity == ARRAY_SEMI_DYNAMIC) {
        if (a->count * a->slot_size > a->capacity) {
            a->capacity *= 2;
            a->slots = realloc(a->slots, a->capacity);
            assert(a->slots && "Cannot realloc!");
#ifdef ARRAY_DEBUG_LOG
            printf("Increased capacity to %ld bytes\n", a->capacity);
#endif
        }
    } else {
        assert(a->count * a->slot_size <= a->capacity && "Array overflow!");
    }

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
    if (a->dynamicity == ARRAY_DYNAMIC || a->dynamicity == ARRAY_SEMI_DYNAMIC) {
        if (a->count > 0 && a->count * a->slot_size <= a->capacity / 4) {
            a->capacity = a->count * a->slot_size * 2;
            a->slots = realloc(a->slots, a->capacity);
            assert(a->slots && "Cannot realloc!");
#ifdef ARRAY_DEBUG_LOG
            printf("Decreased capacity to %ld bytes\n", a->capacity);
#endif
        }
    }
}

void array_clear(array* a) {
    assert(a && "Passed NULL array pointer!");
    a->count = 0;
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

char array_empty(array* a) {
    return (array_size(a) == 0);
}

void array_sort(array* a, int (*compare_func)(const void*, const void*)) {
    qsort(a->slots, a->count, a->slot_size, compare_func);
}

void* array_find(array* a, const void* value, int (*compare_func)(const void*, const void*)) {
    for (size_t i = 0; i < a->count; ++i) {
        void* e = array_at(a, i);
        if (compare_func(e, value)) {
            return e;
        }
    }
    return NULL;
}


char array_binary_search(array* a, const void* value, int (*compare_func)(const void*, const void*), size_t* pos) {
    assert(a && "Passed NULL array pointer!");
    assert(value && "Passed NULL value");
    assert(compare_func && "Passed NULL compare function");
    assert(pos && "Passed NULL position variable");
    if (array_empty(a)){
        *pos = 0;
        return 0;
    }

    size_t left = 0;
    size_t right = array_size(a) - 1;
    while (left <= right) {
        size_t mid = left + (right - left) / 2;
        const void* element = array_at(a, mid);

        // compare the element with the value to search
        int compare_res = compare_func(value, element);

        // element found
        if (compare_res == 0){
            *pos = mid;
            return 1;
        }

        // value is smaller, ignore right half
        if (compare_res < 0) {
            if (mid == 0) {
                break;
            }
            right = mid - 1;
        }
        // value is bigger, ignore left half 
        else {
            left = mid + 1;
        }
    }

    // element was not found, return the optimal insert position
    *pos = left;
    return 0;
}

#endif  // ARRAY_IMPLEMENTATION

#endif  // __ARRAY_H__