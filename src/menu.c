#include "menu.h"
#include "os.h"
#include "melee.h"
#include "util.h"
#include <math.h>

#define TEXT_ID 70
#define TEXT_X 0
#define TEXT_Y 250
#define TEXT_WIDTH 49
#define TEXT_HEIGHT 20
#define TEXT_SCALE_X 9.F
#define TEXT_SCALE_Y 12.F
#define TEXT_COLOR0 0xFFFFFFFF
#define TEXT_COLOR1 0xFFFF40FF
#define TEXT_COLOR2 0x00C000FF
#define TEXT_COLOR3 0x00FF00FF
#define TEXT_BGCOLOR 0x00000080

#define REPEAT_DELAY 20
#define REPEAT_INTERVAL 5

#define COLOR_UNSELECTED 0
#define COLOR_SELECTED 1
#define COLOR_TEXT 2
#define COLOR_HEADER 3

const char *on_off_text[] = { "Off", "On" };

const char *develop_toggle_text[] = {
	"Off",
	NULL,
	NULL,
	NULL,
	"On"
};

static DevText *menu_text;
static char text_buf[TEXT_WIDTH * TEXT_HEIGHT * 2];

static BOOL menu_open = FALSE;
static int menu_port = 0;

static Direction held_direction;
static int direction_hold_frames = 0;

Menu main_menu;
static Menu *current_menu = &main_menu;

static Direction GetDirection(float x, float y)
{
	if (fabs(x) < DEADZONE && fabs(y) < DEADZONE)
		return Dir_None;

	if (fabs(y) > fabs(x))
		return y > 0 ? Dir_Up : Dir_Down;
	else
		return x > 0 ? Dir_Right : Dir_Left;
}

static Direction UpdateInputDirection(HSD_PadStatus *pad)
{
	Direction cstick = GetDirection(pad->cstick_x, pad->cstick_y);
	if (cstick != Dir_None) {
		// Cstick repeats every frame
		held_direction = Dir_None;
		direction_hold_frames = 0;
		return cstick;
	}

	Direction direction = GetDirection(pad->stick_x, pad->stick_y);
	if (direction != held_direction || direction == Dir_None) {
		held_direction = direction;
		direction_hold_frames = 0;
		return direction;
	}

	int repeat_frames = ++direction_hold_frames - REPEAT_DELAY;
	if (repeat_frames < 0 || repeat_frames % REPEAT_INTERVAL != 0)
		return Dir_None;
	else
		return direction;
}

static void Menu_OpenClose(void)
{
	if (menu_open) {
		HSD_PadStatus *pad = &HSD_PadMasterStatus[menu_port];
		if (pad->buttons != Button_DPadDown)
			return;

		if (pad->instant_buttons != Button_DPadDown)
			return;

		DevelopText_HideText(menu_text);
		DevelopText_HideBackground(menu_text);
		menu_open = FALSE;
		return;
	}

	for (int i = 0; i < 4; i++) {
		Button buttons = HSD_PadMasterStatus[i].instant_buttons;
		HSD_PadStatus *pad = &HSD_PadMasterStatus[i];
		if (pad->buttons != Button_DPadDown)
			return;

		if (pad->instant_buttons != Button_DPadDown)
			return;

		// The main menu will have a NULL selection on first use
		if (current_menu->selected == NULL)
			current_menu->selected = current_menu->items.next;

		DevelopText_ShowText(menu_text);
		DevelopText_ShowBackground(menu_text);
		menu_open = TRUE;
		menu_port = i;
	}
}

static void Menu_UpdateSelection(Direction direction)
{
	if (direction != Dir_Down && direction != Dir_Up)
		return;

	Menu *menu = current_menu;
	ListLink *selected = menu->selected;

	while (1) {
		if (direction == Dir_Down)
			selected = selected->next;
		else
			selected = selected->prev;

		// Reached the end
		if (selected == &menu->items)
			return;

		// Skip text items
		MenuItem *item = (MenuItem*)selected;
		if (item->type != MenuItem_Text) {
			menu->selected = selected;
			return;
		}
	}
}

static void Menu_InputForSelectedItem(Direction direction)
{
	HSD_PadStatus *pad = &HSD_PadMasterStatus[menu_port];
	Menu *menu = current_menu;
	MenuItem *item = (MenuItem*)menu->selected;
	BOOL pressed_a = (pad->instant_buttons & Button_A) != 0;

	switch (item->type) {
	case MenuItem_Callback:
		if (pressed_a)
			item->u.callback(item, menu_port);

		break;

	case MenuItem_SubMenu:
		if (!pressed_a)
			break;

		item->u.submenu->previous_menu = menu;
		current_menu = item->u.submenu;
		// Select first item
		current_menu->selected = &current_menu->items;
		Menu_UpdateSelection(Dir_Down);
		break;

	case MenuItem_AdjustInt:
	case MenuItem_AdjustEnum: {
		int *value = item->u.adjust_int.value;
		int increment = item->u.adjust_int.increment;
		int min = item->u.adjust_int.min;
		int max = item->u.adjust_int.max;
		BOOL wrap = item->u.adjust_int.wrap;

		if (direction == Dir_Right || pressed_a) {
			*value += increment;
			if (*value > max)
				*value = wrap ? min : max;
		} else if (direction == Dir_Left) {
			*value -= increment;
			if (*value < min)
				*value = wrap ? max : min;
		}

		break;
	}
	case MenuItem_AdjustFloat: {
		float *value = item->u.adjust_float.value;
		float increment = item->u.adjust_float.increment;
		float min = item->u.adjust_float.min;
		float max = item->u.adjust_float.max;
		BOOL wrap = item->u.adjust_float.wrap;

		if (direction == Dir_Right || pressed_a) {
			*value += increment;
			if (*value > max)
				*value = wrap ? min : max;
		} else if (direction == Dir_Left) {
			*value -= increment;
			if (*value < min)
				*value = wrap ? max : min;
		}

		break;
	}
	}
}

static void DrawItem(MenuItem *item, BOOL selected)
{
	const char *text = item->text;

	if (item->type == MenuItem_Text) {
		DevelopText_StoreColorIndex(menu_text, COLOR_TEXT);
		// Don't use printf, it may overflow with multiline text
		DevelopText_Print(menu_text, text);
		DevelopText_Print(menu_text, "\n");
		return;
	}

	if (selected) {
		DevelopText_StoreColorIndex(menu_text, COLOR_SELECTED);
		DevelopText_Print(menu_text, "- ");
	} else {
		DevelopText_StoreColorIndex(menu_text, COLOR_UNSELECTED);
		DevelopText_Print(menu_text, "  ");
	}

	switch (item->type) {
	case MenuItem_Callback:
		DevelopText_Printf(menu_text, "%s\n", text);
		break;

	case MenuItem_SubMenu:
		DevelopText_Printf(menu_text, "%s>\n", text);
		break;

	case MenuItem_AdjustInt: {
		int value = *item->u.adjust_int.value;
		DevelopText_Printf(menu_text, "%-22s%24d\n", text, value);
		break;
	}
	case MenuItem_AdjustEnum: {
		int value = *item->u.adjust_enum.value;
		const char *name = item->u.adjust_enum.enum_names[value];
		DevelopText_Printf(menu_text, "%-22s%24s\n", text, name);
		break;
	}
	case MenuItem_AdjustFloat: {
		float value = *item->u.adjust_float.value;
		DevelopText_Printf(menu_text, "%-22s%024.04f\n", text, value);
		break;
	}
	}
}

static void Menu_Draw(void)
{
	DevelopText_Erase(menu_text);
	DevelopText_ResetCursorXY(menu_text, 0, 0);

	Menu *menu = current_menu;
	DevelopText_StoreColorIndex(menu_text, COLOR_HEADER);
	DevelopText_Printf(menu_text, "%s\n\n", menu->name);

	ListLink *elem = menu->items.next;
	while (elem != &menu->items) {
		MenuItem *item = (MenuItem*)elem;
		DrawItem(item, menu->selected == elem);
		elem = elem->next;
	}
}

void Menu_Update(void)
{
	Menu_OpenClose();
	if (!menu_open)
		return;

	HSD_PadStatus *pad = &HSD_PadMasterStatus[menu_port];
	if (pad->instant_buttons & Button_B) {
		// Back out of menu
		Menu *previous_menu = current_menu->previous_menu;
		if (previous_menu != NULL) {
			current_menu = previous_menu;
		}
	}

	Direction direction = UpdateInputDirection(pad);
	Menu_UpdateSelection(direction);
	Menu_InputForSelectedItem(direction);
	Menu_Draw();
}

void Menu_Init(Menu *menu, const char *name)
{
	menu->name = name;
	List_Init(&menu->items);
}

void Menu_AddItem(Menu *menu, MenuItem *item)
{
	List_Append(&menu->items, &item->link);
}

void Menu_RemoveItem(MenuItem *item)
{
	List_Remove(&item->link);
}

void Menu_CreateText(void)
{
	menu_text = DevelopText_Initialize(
		TEXT_ID, TEXT_X, TEXT_Y, TEXT_WIDTH, TEXT_HEIGHT, text_buf);

	DevelopText_AddToMainList(NULL, menu_text);
	DevelopText_ResetUnknownFlag(menu_text);
	DevelopText_StoreBGColor(menu_text, &(u32) { TEXT_BGCOLOR });
	DevelopText_StoreTextScale(menu_text, TEXT_SCALE_X, TEXT_SCALE_Y);

	DevelopText_StoreColorIndex(menu_text, 3);
	DevelopText_StoreTextColor(menu_text, &(u32) { TEXT_COLOR3 });
	DevelopText_StoreColorIndex(menu_text, 2);
	DevelopText_StoreTextColor(menu_text, &(u32) { TEXT_COLOR2 });
	DevelopText_StoreColorIndex(menu_text, 1);
	DevelopText_StoreTextColor(menu_text, &(u32) { TEXT_COLOR1 });
	DevelopText_StoreColorIndex(menu_text, 0);
	DevelopText_StoreTextColor(menu_text, &(u32) { TEXT_COLOR0 });

	DevelopText_HideText(menu_text);
	DevelopText_HideBackground(menu_text);
}

BOOL IsMenuOpen(void)
{
	return menu_open;
}

void MainMenu_Init(void)
{
	static MenuItem menu_dblevel = {
		.text = "Develop Mode",
		.type = MenuItem_AdjustEnum,
		.u = {
			.adjust_enum = {
				&DbLevel,
				DbLevel_Develop, 0, DbLevel_Develop, TRUE,
				develop_toggle_text
			}
		}
	};

	Menu_Init(&main_menu, "Lab Tools");
	Menu_AddItem(&main_menu, &menu_dblevel);
}