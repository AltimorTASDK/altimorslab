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

#define PAGE_SIZE 16

#define COLOR_UNSELECTED 0
#define COLOR_SELECTED 1
#define COLOR_TEXT 2
#define COLOR_HEADER 3

const char *OnOffText[] = { "Off", "On" };

const char *develop_toggle_text[] = {
	"Off",
	"1",
	"2",
	"3",
	"On"
};

static DevText *menu_text;
static char text_buf[TEXT_WIDTH * TEXT_HEIGHT * 2];

static BOOL menu_open = FALSE;
static int menu_port = 0;

static Direction held_direction;
static int direction_hold_frames = 0;

Menu MainMenu;
Menu *CurrentMenu = &MainMenu;

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
		HSD_PadStatus *pad = &HSD_PadMasterStatus[i];
		if (pad->buttons != Button_DPadDown)
			return;

		if (pad->instant_buttons != Button_DPadDown)
			return;

		// The main menu will have a NULL selection on first use
		if (CurrentMenu->selected == NULL) {
			CurrentMenu->selected = CurrentMenu->items.next;
			CurrentMenu->scroll_item = CurrentMenu->items.next;
			CurrentMenu->scroll_index = 0;
		}

		DevelopText_ShowText(menu_text);
		DevelopText_ShowBackground(menu_text);
		menu_open = TRUE;
		menu_port = i;
	}
}

static void Menu_UpdateVisibleItems(Menu *menu)
{
	int lines = 0;
	ListLink *elem = menu->scroll_item;

	menu->visible_items = 0;

	while (elem != &menu->items && lines < PAGE_SIZE) {
		MenuItem *item = (MenuItem*)elem;
		lines += item->line_count;
		if (lines <= PAGE_SIZE)
			menu->visible_items++;

		elem = elem->next;
	}
}

static void Menu_UpdateSelection(Direction direction)
{
	if (direction != Dir_Down && direction != Dir_Up)
		return;

	Menu *menu = CurrentMenu;
	ListLink *selected = menu->selected;

	while (1) {
		if (direction == Dir_Down)
			selected = selected->next;
		else
			selected = selected->prev;

		// Reached the end
		if (selected == &menu->items)
			return;

		MenuItem *item = (MenuItem*)selected;

		// Scroll
		if (item->index < menu->scroll_index) {
			for (int i = 0; i < item->line_count; i++) {
				menu->scroll_item = menu->scroll_item->prev;
				menu->scroll_index--;
			}
			Menu_UpdateVisibleItems(menu);
		} else if (item->index >= menu->scroll_index + menu->visible_items) {
			for (int i = 0; i < item->line_count; i++) {
				menu->scroll_item = menu->scroll_item->next;
				menu->scroll_index++;
			}
			Menu_UpdateVisibleItems(menu);
		}

		// Can't select text items
		if (item->type == MenuItem_Text)
			continue;

		menu->selected = selected;
		return;
	}
}

static void Menu_ItemCallback(MenuItem *item)
{
	if (item->callback != NULL)
		item->callback(item, menu_port);
}

static void Menu_InputForSelectedItem(Direction direction)
{
	const HSD_PadStatus *pad = &HSD_PadMasterStatus[menu_port];
	Menu *menu = CurrentMenu;
	MenuItem *item = (MenuItem*)menu->selected;
	BOOL pressed_a = (pad->instant_buttons & Button_A) != 0;

	switch (item->type) {
	case MenuItem_Callback:
		if (pressed_a)
			Menu_ItemCallback(item);

		break;

	case MenuItem_SubMenu:
		if (!pressed_a)
			break;

		Menu_ItemCallback(item);
		item->u.submenu->previous_menu = menu;
		CurrentMenu = item->u.submenu;
		// Select first item
		CurrentMenu->selected = &CurrentMenu->items;
		CurrentMenu->scroll_item = CurrentMenu->items.next;
		CurrentMenu->scroll_index = 0;
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

			Menu_ItemCallback(item);
		} else if (direction == Dir_Left) {
			*value -= increment;
			if (*value < min)
				*value = wrap ? max : min;

			Menu_ItemCallback(item);
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

			Menu_ItemCallback(item);
		} else if (direction == Dir_Left) {
			*value -= increment;
			if (*value < min)
				*value = wrap ? max : min;

			Menu_ItemCallback(item);
		}

		break;
	}
	}
}

static void DrawItem(MenuItem *item, BOOL selected)
{
	const char *text = item->text;
	u8 color = selected ? COLOR_SELECTED : COLOR_UNSELECTED;

	if (item->type == MenuItem_Text || item->type == MenuItem_TextSelectable) {
		if (item->type == MenuItem_TextSelectable)
			DevelopText_StoreColorIndex(menu_text, color);
		else
			DevelopText_StoreColorIndex(menu_text, COLOR_TEXT);

		// Don't use printf, it may overflow with multiline text
		DevelopText_Print(menu_text, text);
		DevelopText_Print(menu_text, "\n");
		return;
	}

	DevelopText_StoreColorIndex(menu_text, color);

	if (selected)
		DevelopText_Print(menu_text, "- ");
	else
		DevelopText_Print(menu_text, "  ");

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
	DevelopText_SetCursorXY(menu_text, 0, 0);

	Menu *menu = CurrentMenu;
	DevelopText_StoreColorIndex(menu_text, COLOR_HEADER);
	DevelopText_Printf(menu_text, "%s\n\n", menu->name);

	if (menu->scroll_index > 0)
		DevelopText_Print(menu_text, "More Above\n");
	else
		DevelopText_Print(menu_text, "\n");

	int count = 0;
	ListLink *elem = menu->scroll_item;

	while (elem != &menu->items && count++ < menu->visible_items) {
		MenuItem *item = (MenuItem*)elem;
		DrawItem(item, menu->selected == elem);
		elem = elem->next;
	}

	DevelopText_StoreColorIndex(menu_text, COLOR_HEADER);

	if (menu->scroll_index + menu->visible_items < menu->item_count)
		DevelopText_Print(menu_text, "More Below");
}

void Menu_Update(void)
{
	Menu_OpenClose();
	if (!menu_open)
		return;

	HSD_PadStatus *pad = &HSD_PadMasterStatus[menu_port];
	if (pad->instant_buttons & Button_B) {
		// Back out of menu
		Menu *previous_menu = CurrentMenu->previous_menu;
		if (previous_menu != NULL)
			CurrentMenu = previous_menu;
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
	menu->item_count = 0;
}

static int GetItemLineCount(MenuItem *item)
{
	if (item->type != MenuItem_Text && item->type != MenuItem_TextSelectable)
		return 1;

	int count = 1;
	int x = 0;
	for (const char *c = item->text; *c != '\0'; c++) {
		if (*c == '\n' || ++x == TEXT_WIDTH) {
			count++;
			x = 0;
		}
	}

	return count;
}

void Menu_AddItem(Menu *menu, MenuItem *item)
{
	List_Append(&menu->items, &item->link);
	item->index = menu->item_count++;
	if (item->index == 0)
		menu->scroll_item = &item->link;

	item->line_count = GetItemLineCount(item);
	Menu_UpdateVisibleItems(menu);
}

void Menu_RemoveItem(Menu *menu, MenuItem *item)
{
	ListLink *next = item->link.next;
	while (next != &menu->items) {
		((MenuItem*)next)->index--;
		next = next->next;
	}

	if (menu->scroll_index > item->index) {
		menu->scroll_index--;
	} else if (menu->scroll_index == item->index) {
		if (item->link.next == &menu->items) {
			menu->scroll_item = item->link.prev;
			menu->scroll_index--;
		} else {
			menu->scroll_item = item->link.next;
		}
	}

	if (menu->selected == &item->link) {
		if (item->link.next == &menu->items)
			menu->selected = item->link.prev;
		else
			menu->selected = item->link.next;
	}

	List_Remove(&item->link);
	menu->item_count--;

	Menu_UpdateVisibleItems(menu);
}

void Menu_RemoveAllItems(Menu *menu)
{
    List_Init(&menu->items);
    menu->item_count = 0;
	menu->scroll_index = 0;
	menu->scroll_item = &menu->items;
	menu->visible_items = 0;
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
				1, 0, DbLevel_Develop, TRUE,
				develop_toggle_text
			}
		}
	};

	Menu_Init(&MainMenu, "Lab Tools");
	Menu_AddItem(&MainMenu, &menu_dblevel);
}