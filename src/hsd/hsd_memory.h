#pragma once

#include <cstddef>

extern "C" {

void *HSD_MemAlloc(size_t size);
void HSD_Free(void *ptr);

}