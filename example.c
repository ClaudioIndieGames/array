#include <stdio.h>
#define ARRAY_DEBUG_LOG
#include "array.h"

int main() {
    // dynamic array example
    array a1;
    array_dynamic_init(&a1, sizeof(int), 10);
    for (int i = 0; i < 15; ++i) {
        array_push_back(&a1, &i);
    }
    for (int i = 14; i >= 0; --i) {
        array_pop_front(&a1);
    }
    array_dynamic_fini(&a1);

    // static array example
    array a2;
    int container[10];
    array_static_init(&a2, &container, sizeof(int), sizeof(container) / sizeof(int));
    for (int i = 0; i < 10; ++i) {
        array_push_back(&a2, &i);
    }
    for (int i = 9; i >= 0; --i) {
        array_pop_front(&a2);
    }
    return 0;
}