#include "os.h"
#include "hsd.h"
#include "melee.h"
#include "log_display.h"
#include "menu.h"
#include "overlays.h"
#include "hitbox_stats.h"

static BOOL initialized = FALSE;

void orig_ActionStateChange(
	HSD_GObj *gobj, u32 new_state, u32 flags, HSD_GObj *gobj2,
	float start_frame, float frame_rate, float param_7);
void hook_ActionStateChange(
	HSD_GObj *gobj, u32 new_state, u32 flags, HSD_GObj *gobj2,
	float start_frame, float frame_rate, float param_7)
{
	orig_ActionStateChange(
		gobj, new_state, flags, gobj2, start_frame, frame_rate, param_7);

	Overlays_ASChange(gobj->data, new_state);
}

HSD_GObj *orig_AllocateAndInitPlayer(void *player_block);
HSD_GObj *hook_AllocateAndInitPlayer(void *player_block)
{
	HSD_GObj *gobj = orig_AllocateAndInitPlayer(player_block);
	//GObj_CreateProcWithCallback(gobj, HitboxStats_UpdatePlayer, 0x17);
	return gobj;
}

void orig_wP_RunObjectFrameFunctions(void);
void hook_wP_RunObjectFrameFunctions(void)
{
	// Pause game when menu is open
	if (IsMenuOpen())
		return;

	orig_wP_RunObjectFrameFunctions();
	HitboxStats_Update();
}


void orig_DevelopMode_IngameTogglesMost(void);
void hook_DevelopMode_IngameTogglesMost(void)
{
	orig_DevelopMode_IngameTogglesMost();

	if (!initialized)
		return;

	LogDisplay_Update();
	Menu_Update();
}

void orig_StartMelee(void *param_1);
void hook_StartMelee(void *param_1)
{
	initialized = FALSE;

	orig_StartMelee(param_1);

	LogDisplay_CreateText();
	Menu_CreateText();
	HitboxStats_SceneInit();

	initialized = TRUE;
}

void _start(void)
{
	MainMenu_Init();
	Overlays_Init();
	HitboxStats_Init();

	start();
}