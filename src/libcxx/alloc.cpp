#include "hsd/memory.h"
#include <new>

// Explicitly use heap 0 because it doesn't get destroyed on scene transition

void *operator new(size_t count)
{
	return HSD_MemAllocFromHeap(0, count);
}

void *operator new[](size_t count)
{
	return HSD_MemAllocFromHeap(0, count);
}

void operator delete(void *ptr)
{
	HSD_FreeToHeap(0, ptr);
}

void operator delete(void *ptr, size_t sz)
{
	HSD_FreeToHeap(0, ptr);
}

void operator delete[](void *ptr)
{
	HSD_FreeToHeap(0, ptr);
}

void operator delete[](void *ptr, size_t sz)
{
	HSD_FreeToHeap(0, ptr);
}