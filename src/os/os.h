#pragma once

#include "util/int_types.h"
#include <cstddef>

#define HEAP_DEFAULT (-1)

#define PANIC(...) OSPanic(__FILE__, __LINE__, __VA_ARGS__)

extern "C" {

// Audio heap doesn't get destroyed on major scene transition
void *Audio_MemAlloc(size_t size);
void Audio_Free(void *ptr);

__attribute__((noreturn)) void OSPanic(const char *file, int line, const char *fmt, ...);
void OSReport(const char *fmt, ...);
void *OSAllocFromHeap(int heap, size_t size);
u64 __OSGetSystemTime();

}