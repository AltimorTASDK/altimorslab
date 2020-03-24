#pragma once

#include <stddef.h>

#define HEAP_DEFAULT (-1)

void(*OSReport)(const char *fmt, ...) = (void*)0x803456A8;
void*(*OSAllocFromHeap)(int heap, size_t size) = (void*)0x80343EF0;