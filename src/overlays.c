#include "menu.h"
#include "melee.h"
#include "util.h"

#define MAX_OVERLAYS (SpecialOverlay_Max + 64)

typedef struct _Overlay {
	int action_state;
	int r, g, b, a;
	char name[64];
	MenuItem menu_overlay_name;
	MenuItem menu_action_state;
	MenuItem menu_r;
	MenuItem menu_g;
	MenuItem menu_b;
	MenuItem menu_a;
} Overlay;

typedef enum _SpecialOverlay {
	SpecialOverlay_Airborne,
	SpecialOverlay_FaceUp,
	SpecialOverlay_FaceDown,
	SpecialOverlay_Max
} SpecialOverlay;

static Menu overlay_menu;
static Overlay overlays[MAX_OVERLAYS];
static int overlay_count = SpecialOverlay_Max;

static const char *special_overlay_names[] = {
	"Airborne",
	"Face Up",
	"Face Down"
};

static void AddOverlayToMenu(Overlay *overlay)
{
	Menu_AddItem(&overlay_menu, &overlay->menu_overlay_name);
	Menu_AddItem(&overlay_menu, &overlay->menu_action_state);
	Menu_AddItem(&overlay_menu, &overlay->menu_r);
	Menu_AddItem(&overlay_menu, &overlay->menu_g);
	Menu_AddItem(&overlay_menu, &overlay->menu_b);
	Menu_AddItem(&overlay_menu, &overlay->menu_a);
}

static void AddASOverlay(MenuItem *item, int port)
{
	Overlay *overlay = &overlays[overlay_count++];

	// Defaults
	overlay->action_state = AS_Wait;
	overlay->r = overlay->g = overlay->b = overlay->a = 0;
}

static void RemoveASOverlay(MenuItem *item, int port)
{
	if (overlay_count == SpecialOverlay_Max)
		return;

	Overlay *overlay = &overlays[--overlay_count];
	Menu_RemoveItem(&overlay->menu_overlay_name);
	Menu_RemoveItem(&overlay->menu_action_state);
	Menu_RemoveItem(&overlay->menu_r);
	Menu_RemoveItem(&overlay->menu_g);
	Menu_RemoveItem(&overlay->menu_b);
	Menu_RemoveItem(&overlay->menu_a);
}

static void InitOverlay(int index)
{
	Overlay *overlay = &overlays[index];
	int as_overlay_index = index - SpecialOverlay_Max;

	if (as_overlay_index < 0)
		sprintf(overlay->name, "\n%s Overlay", special_overlay_names[index]);
	else
		sprintf(overlay->name, "\nAction State Overlay %d", as_overlay_index);

	overlay->menu_overlay_name = (MenuItem) {
		.text = overlay->name,
		.type = MenuItem_Text
	};

	overlay->menu_action_state = (MenuItem) {
		.text = "Action State",
		.type = MenuItem_AdjustEnum,
		.u = {
			.adjust_enum = {
				&overlay->action_state,
				1, 0, AS_NAMED_MAX - 1, FALSE,
				action_state_names
			}
		}
	};

	overlay->menu_r = (MenuItem) {
		.text = "Red",
		.type = MenuItem_AdjustInt,
		.u = { .adjust_int = { &overlay->r, 5, 0, 255, FALSE } }
	};

	overlay->menu_g = (MenuItem) {
		.text = "Green",
		.type = MenuItem_AdjustInt,
		.u = { .adjust_int = { &overlay->g, 5, 0, 255, FALSE } }
	};

	overlay->menu_b = (MenuItem) {
		.text = "Blue",
		.type = MenuItem_AdjustInt,
		.u = { .adjust_int = { &overlay->b, 5, 0, 255, FALSE } }
	};

	overlay->menu_a = (MenuItem) {
		.text = "Alpha",
		.type = MenuItem_AdjustInt,
		.u = { .adjust_int = { &overlay->a, 5, 0, 255, FALSE } }
	};

	if (as_overlay_index < 0)
		AddOverlayToMenu(overlay);
}

/*
 * Checks which way a player will face when they land from tumble
 */
static BOOL CheckKnockdownFacingOverlay(Player *player, BOOL face_up)
{
	// Check tumble
	u32 as = player->action_state;
	if (as != AS_DamageFall && (as < AS_DamageFlyHi || as > AS_DamageFlyRoll))
		return FALSE;

	return Player_IsKnockdownFaceUp(player->gobj) == face_up;
}

static BOOL ShouldApplyOverlay(Player *player, int index)
{
	Overlay *overlay = &overlays[index];

	switch (index) {
	case SpecialOverlay_Airborne:
		return player->airborne;
	case SpecialOverlay_FaceUp:
		return CheckKnockdownFacingOverlay(player, TRUE);
	case SpecialOverlay_FaceDown:
		return CheckKnockdownFacingOverlay(player, FALSE);
	default:
		return player->action_state == overlay->action_state;
	}
}

void orig_ColorData_HandleStuff(Player *player, u32 param_2, u32 param_3);
void hook_ColorData_HandleStuff(Player *player, u32 param_2, u32 param_3)
{
	float r = 0.f, g = 0.f, b = 0.f, a = 0.f;

	for (int i = 0; i < overlay_count; i++)
	{
		Overlay *overlay = &overlays[i];

		if (overlay->a == 0 || !ShouldApplyOverlay(player, i))
			continue;

		// Blend overlay colors together
		float src_alpha = (float)overlay->a / 255.f;
		float inv_src_alpha = 1.f - src_alpha;
		r = (float)overlay->r / 255.f * src_alpha + r * inv_src_alpha;
		g = (float)overlay->g / 255.f * src_alpha + g * inv_src_alpha;
		b = (float)overlay->b / 255.f * src_alpha + b * inv_src_alpha;
		a = (float)overlay->a / 255.f + a * inv_src_alpha;
	}

	if (a == 0.f) {
		// No visible overlays
		orig_ColorData_HandleStuff(player, param_2, param_3);
		return;
	}

	u8 old_flags12 = player->flags12;
	u8 old_override_color_r = player->override_color_r;
	u8 old_override_color_g = player->override_color_g;
	u8 old_override_color_b = player->override_color_b;
	u8 old_override_color_a = player->override_color_a;

	player->flags12 |= PlayerFlag12_OverrideColor;
	player->override_color_r = (u8)(r * 255.f);
	player->override_color_g = (u8)(g * 255.f);
	player->override_color_b = (u8)(b * 255.f);
	player->override_color_a = (u8)(a * 255.f);

	orig_ColorData_HandleStuff(player, param_2, param_3);

	player->flags12 = old_flags12;
	player->override_color_r = old_override_color_r;
	player->override_color_g = old_override_color_g;
	player->override_color_b = old_override_color_b;
	player->override_color_a = old_override_color_a;
}

void Overlays_Init(void)
{
	static MenuItem menu_add_overlay = {
		.text = "Add Action State Overlay",
		.type = MenuItem_Callback,
		.callback = AddASOverlay
	};

	static MenuItem menu_remove_overlay = {
		.text = "Remove Action State Overlay",
		.type = MenuItem_Callback,
		.callback = RemoveASOverlay
	};

	Menu_Init(&overlay_menu, "Overlays");

	// Also adds special overlays to menu
	for (int i = 0; i < MAX_OVERLAYS; i++)
		InitOverlay(i);

	Menu_AddItem(&overlay_menu, &menu_add_overlay);
	Menu_AddItem(&overlay_menu, &menu_remove_overlay);

	// Add to main menu
	static MenuItem menu_overlays = {
		.text = "Overlays",
		.type = MenuItem_SubMenu,
		.u = { .submenu = &overlay_menu }
	};

	Menu_AddItem(&MainMenu, &menu_overlays);
}