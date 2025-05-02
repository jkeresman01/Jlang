#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void* jalloc(uint64_t size) {
    return malloc((size_t)size);
}

void jfree(void* ptr) {
    free(ptr);
}

void print(const char* str) {
    puts(str);
}

bool is_null(void* ptr) {
    return ptr == NULL;
}

