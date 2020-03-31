#include "menu.h"
#include "melee.h"
#include "util.h"

#define MAX_OVERLAYS 64

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

static Menu overlay_menu;
static Overlay as_overlays[MAX_OVERLAYS];
static int as_overlay_count = 0;

static void AddASOverlay(MenuItem *item, int port)
{
	Overlay *overlay = &as_overlays[as_overlay_count++];

	// Defaults
	overlay->action_state = AS_Wait;
	overlay->r = overlay->g = overlay->b = overlay->a = 0;

	Menu_AddItem(&overlay_menu, &overlay->menu_overlay_name);
	Menu_AddItem(&overlay_menu, &overlay->menu_action_state);
	Menu_AddItem(&overlay_menu, &overlay->menu_r);
	Menu_AddItem(&overlay_menu, &overlay->menu_g);
	Menu_AddItem(&overlay_menu, &overlay->menu_b);
	Menu_AddItem(&overlay_menu, &overlay->menu_a);
}

static void RemoveASOverlay(MenuItem *item, int port)
{
	Overlay *overlay = &as_overlays[--as_overlay_count];
	Menu_RemoveItem(&overlay->menu_overlay_name);
	Menu_RemoveItem(&overlay->menu_action_state);
	Menu_RemoveItem(&overlay->menu_r);
	Menu_RemoveItem(&overlay->menu_g);
	Menu_RemoveItem(&overlay->menu_b);
	Menu_RemoveItem(&overlay->menu_a);
}

static void Overlays_AddMenu(void)
{
	static MenuItem menu_add_overlay = {
		.text = "Add Action State Overlay",
		.type = MenuItem_Callback,
		.u = { .callback = AddASOverlay }
	};

	static MenuItem menu_remove_overlay = {
		.text = "Remove Action State Overlay",
		.type = MenuItem_Callback,
		.u = { .callback = RemoveASOverlay }
	};

	Menu_Init(&overlay_menu, "Overlays");
	Menu_AddItem(&overlay_menu, &menu_add_overlay);
	Menu_AddItem(&overlay_menu, &menu_remove_overlay);

	// Add to main menu
	static MenuItem menu_overlays = {
		.text = "Overlays",
		.type = MenuItem_SubMenu,
		.u = { .submenu = &overlay_menu }
	};

	Menu_AddItem(&main_menu, &menu_overlays);
}

static void InitOverlay(int index)
{
	Overlay *overlay = &as_overlays[index];

	sprintf(overlay->name, "\nAction State Overlay %d", index);
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
}

void orig_ColorData_HandleStuff(Player *player, u32 param_2, u32 param_3);
void hook_ColorData_HandleStuff(Player *player, u32 param_2, u32 param_3)
{
	for (int i = 0; i < as_overlay_count; i++)
	{
		Overlay *overlay = &as_overlays[i];
		if (overlay->action_state != player->action_state)
			continue;

		u8 old_render_flags = player->render_flags;
		u8 old_override_color_r = player->override_color_r;
		u8 old_override_color_g = player->override_color_g;
		u8 old_override_color_b = player->override_color_b;
		u8 old_override_color_a = player->override_color_a;

		player->render_flags |= RenderFlag_OverrideColor;
		player->override_color_r = overlay->r;
		player->override_color_g = overlay->g;
		player->override_color_b = overlay->b;
		player->override_color_a = overlay->a;

		orig_ColorData_HandleStuff(player, param_2, param_3);

		player->render_flags = old_render_flags;
		player->override_color_r = old_override_color_r;
		player->override_color_g = old_override_color_g;
		player->override_color_b = old_override_color_b;
		player->override_color_a = old_override_color_a;

		return;
	}

	orig_ColorData_HandleStuff(player, param_2, param_3);
}

void Overlays_Update(HSD_GObj *gobj)
{
	Player *player = gobj->data;

}

void Overlays_Init(void)
{
	Overlays_AddMenu();

	for (int i = 0; i < MAX_OVERLAYS; i++)
		InitOverlay(i);
}