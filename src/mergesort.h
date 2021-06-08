#pragma once

#include <stdlib.h>

typedef int (*compare_func_t)(const void*, const void*, void*);
int msort_r(void* arr, int len, size_t width, compare_func_t compar, void* data); // mergesort.c
