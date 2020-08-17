#pragma once

#include "util.h"

typedef struct _Menu {
	const char *name;
	ListLink items;
	int item_count;
	int scroll_index;
	int visible_items;
	ListLink *scroll_item;
	ListLink *selected;
	struct _Menu *previous_menu;
} Menu;

struct _MenuItem;
typedef struct _MenuItem MenuItem;

typedef void(*MenuCallback)(MenuItem *item, int port);

typedef enum _MenuItemType {
	MenuItem_Text,
	MenuItem_TextSelectable,
	MenuItem_Callback,
	MenuItem_SubMenu,
	MenuItem_AdjustInt,
	MenuItem_AdjustEnum,
	MenuItem_AdjustFloat
} MenuItemType;

struct _MenuItem {
	ListLink link;
	int index;
	const char *text;
	int line_count;
	MenuItemType type;
	MenuCallback callback;
	union {
		Menu *submenu;
		struct {
			int *value;
			int increment;
			int min;
			int max;
			BOOL wrap;
		} adjust_int;
		struct {
			int *value;
			int increment;
			int min;
			int max;
			BOOL wrap;
			const char **enum_names;
		} adjust_enum;
		struct {
			float *value;
			float increment;
			float min;
			float max;
			BOOL wrap;
		} adjust_float;
	} u;
};

extern const char *OnOffText[];
extern Menu MainMenu;
extern Menu *CurrentMenu;

void Menu_CreateText(void);
void Menu_Update(void);
void Menu_Init(Menu *menu, const char *name);
void Menu_AddItem(Menu *menu, MenuItem *item);
void Menu_RemoveItem(Menu *menu, MenuItem *item);
void Menu_RemoveAllItems(Menu *menu);
BOOL IsMenuOpen(void);
void MainMenu_Init(void);