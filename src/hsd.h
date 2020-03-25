#pragma once

#include <stddef.h>

typedef struct _HSD_GObj {
    char pad00[0x2C];
    void *data;
} HSD_GObj;

void *HSD_MemAlloc(size_t size);