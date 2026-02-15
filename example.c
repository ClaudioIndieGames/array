#include <stdio.h>
#define ARRAY_DEBUG_LOG
#include "array.h"

int main() {
    // dynamic array example
    array a1;
    array_dynamic_init(&a1, sizeof(int), 10);
    for (int i = 0; i < 15; ++i) {
        int* slot = array_append_slot(&a1);
        *slot = i;
    }
    for (int i = 14; i >= 0; --i) {
        array_pop(&a1);
    }
    array_dynamic_fini(&a1);

    // static array example
    array a2;
    int container[10];
    array_static_init(&a2, &container, sizeof(int), sizeof(container) / sizeof(int));
    for (int i = 0; i < 10; ++i) {
        array_append_copy(&a2, &i);
    }
    for (int i = 9; i >= 0; --i) {
        array_pop(&a2);
    }
    return 0;
}