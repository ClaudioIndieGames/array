#include <stdio.h>
#define ARRAY_DEBUG_LOG
#include "array.h"

int main() {
    // dynamic array example
    array* a1 = array_create_dynamic(sizeof(int), 10);
    for (int i = 0; i < 15; ++i) {
        int* slot = array_append_slot(a1);
        *slot = i;
    }
    for (int i = 14; i >= 0; --i) {
        array_pop(a1);
    }
    array_destroy(a1);

    // semi-dynamic array example
    array* a2 = array_create_semi_dynamic(&(array){0}, sizeof(int), 10);
    for (int i = 0; i < 15; ++i) {
        int* slot = array_append_slot(a2);
        *slot = i;
    }
    for (int i = 14; i >= 0; --i) {
        array_pop(a2);
    }
    array_destroy(a2);

    // semi-static array example
    array* a3 = array_create_semi_static(&(int [10]){0}, sizeof(int), 10);
    for (int i = 0; i < 10; ++i) {
        array_append_copy(a3, &i);
    }
    for (int i = 9; i >= 0; --i) {
        array_pop(a3);
    }
    array_destroy(a3);

    // static array example
    array* a4 = array_create_static(&(array){0}, &(int [10]){0}, sizeof(int), 10);
    for (int i = 0; i < 10; ++i) {
        array_append_copy(a4, &i);
    }
    for (int i = 9; i >= 0; --i) {
        array_pop(a4);
    }

    return 0;
}