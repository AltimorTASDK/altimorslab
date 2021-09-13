#include "os/os.h"
#include <new>

// Use the audio heap because it doesn't get destroyed on scene transition

void *operator new(size_t count) noexcept
{
	return Audio_MemAlloc(count);
}

void operator delete(void *ptr) noexcept
{
	Audio_Free(ptr);
}