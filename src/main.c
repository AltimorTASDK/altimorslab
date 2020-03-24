#include "os.h"
#include "hsd.h"
#include "melee.h"
#include "log_display.h"

void orig_ActionStateChange(
	HSD_GObj *gobj, int new_state, int param_3, int param_4,
	float param_5, float param_6, float param_7);
void hook_ActionStateChange(
	HSD_GObj *gobj, u32 new_state, int param_3, int param_4,
	float param_5, float param_6, float param_7)
{
	Player *player = gobj->data;

	char from_name[256], to_name[256];
	GetActionStateName(player->action_state, from_name);
	GetActionStateName(new_state, to_name);

	LogDisplay_Printf(
		"%03.f %-24s -> %-24s\n",
		player->frame_timer,
		from_name,
		to_name);

	orig_ActionStateChange(
		gobj, new_state, param_3, param_4, param_5, param_6, param_7);
}

void _start(void)
{
	start();
}