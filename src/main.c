#include "os.h"
#include "hsd.h"
#include "melee.h"
#include "log_display.h"
#include "menu.h"
#include "overlays.h"
#include "hitbox_stats.h"
#include "cpu_control.h"
#include "extras_dair.h"
#include "input_display.h"

static BOOL initialized = FALSE;

void orig_ActionStateChange(
	HSD_GObj *gobj, u32 new_state, u32 flags, HSD_GObj *gobj2,
	float start_frame, float frame_rate, float param_7);
void hook_ActionStateChange(
	HSD_GObj *gobj, u32 new_state, u32 flags, HSD_GObj *gobj2,
	float start_frame, float frame_rate, float lerp_override)
{
	if (!CPUControl_ASChange(gobj->data))
		return;

	ExtrasDair_ASChange(gobj->data, &new_state);

	orig_ActionStateChange(
		gobj, new_state, flags, gobj2, start_frame, frame_rate, lerp_override);
}

void orig_Player_UpdateActionInputTimers(HSD_GObj *gobj);
void hook_Player_UpdateActionInputTimers(HSD_GObj *gobj)
{
	// Hooking here allows running code after inputs are updated, but before
	// the AS interrupt runs
	orig_Player_UpdateActionInputTimers(gobj);
	CPUControl_UpdateInput(gobj->data);
}

void orig_Physics_Move(void *collision_callback, Physics *phys, u32 flags);
void hook_Physics_Move(void *collision_callback, Physics *phys, u32 flags)
{
	CPUControl_PhysicsMove(phys);
	orig_Physics_Move(collision_callback, phys, flags);
}

void orig_DevelopText_DrawAll(HSD_GObj *gobj, RenderPass pass);
void hook_DevelopText_DrawAll(HSD_GObj *gobj, RenderPass pass)
{
	orig_DevelopText_DrawAll(gobj, pass);
	InputDisplay_Draw(pass);
}

void orig_wP_RunObjectFrameFunctions(void);
void hook_wP_RunObjectFrameFunctions(void)
{
	// Pause game when menu is open
	if (!IsMenuOpen())
		orig_wP_RunObjectFrameFunctions();

	HitboxStats_Update();
}


void orig_DevelopMode_IngameTogglesMost(void);
void hook_DevelopMode_IngameTogglesMost(void)
{
	orig_DevelopMode_IngameTogglesMost();

	if (!initialized) {
		LogDisplay_CreateText();
		Menu_CreateText();
		HitboxStats_SceneInit();
		initialized = TRUE;
	}

	LogDisplay_Update();
	Menu_Update();
}

void orig_StartMelee(void *param_1);
void hook_StartMelee(void *param_1)
{
	initialized = FALSE;
	orig_StartMelee(param_1);
}

void orig_main(void *param_1, void *param_2);
void hook_main(void *param_1, void *param_2)
{
	MainMenu_Init();
	Overlays_Init();
	HitboxStats_Init();
	CPUControl_Init();

	orig_main(param_1, param_2);
}