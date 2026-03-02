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