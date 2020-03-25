#include "os.h"
#include "hsd.h"
#include "melee.h"
#include "log_display.h"
#include "menu.h"
#include "overlays.h"

void orig_ActionStateChange(
	HSD_GObj *gobj, u32 new_state, u32 flags, HSD_GObj *gobj2,
	float start_frame, float frame_rate, float param_7);
void hook_ActionStateChange(
	HSD_GObj *gobj, u32 new_state, u32 flags, HSD_GObj *gobj2,
	float start_frame, float frame_rate, float param_7)
{
	Player *player = gobj->data;

	char from_name[256], to_name[256];
	GetActionStateName(player->action_state, from_name);
	GetActionStateName(new_state, to_name);

	LogDisplay_Printf(
		"\x02%03.f \x01%-24s \x04-> \x01%-24s\n",
		player->frame_timer,
		from_name,
		to_name);

	orig_ActionStateChange(
		gobj, new_state, flags, gobj2, start_frame, frame_rate, param_7);

	Overlays_ASChange(player, new_state);
}

void orig_DevelopMode_IngameTogglesMost(void);
void hook_DevelopMode_IngameTogglesMost(void)
{
	orig_DevelopMode_IngameTogglesMost();

	LogDisplay_Update();
	Menu_Update();
}

void orig_StartMelee(void *param_1);
void hook_StartMelee(void *param_1)
{
	orig_StartMelee(param_1);

	LogDisplay_CreateText();
	Menu_CreateText();
}

void _start(void)
{
	MainMenu_Init();
	Overlays_Init();

	start();
}