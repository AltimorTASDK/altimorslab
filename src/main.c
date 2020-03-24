#include "os.h"
#include "hsd.h"
#include "int_types.h"

void orig_Interrupt_AS_CliffWait(struct HSD_GObj *gobj);

void hook_Interrupt_AS_CliffWait(struct HSD_GObj *gobj)
{
	OSReport("Yo what the fuck up from Interrupt_AS_CliffWait\n");
	orig_Interrupt_AS_CliffWait(gobj);
}

void _start(void)
{
	void(*original_entry_point)(void) = (void*)0x8000522C;
	original_entry_point();
}