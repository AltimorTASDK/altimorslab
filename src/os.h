#pragma once

#include <stddef.h>

#define HEAP_DEFAULT (-1)

void OSReport(const char *fmt, ...);
void *OSAllocFromHeap(int heap, size_t size);