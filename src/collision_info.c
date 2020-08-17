#include "os.h"
#include "melee.h"
#include "menu.h"
#include "melee/collision.h"

#define MAX_LINES 256

typedef struct MenuItemLine_ {
    MenuItem base;
    int line;
    char buf[128];
} MenuItemLine;

static Menu collision_info_menu;
static MenuItemLine menu_lines[MAX_LINES];

static void CollisionInfo_Update(MenuItem *item, int port)
{
    Menu_RemoveAllItems(&collision_info_menu);

    if (groundCollParams == NULL)
        return;

    if (groundCollLine == NULL || groundCollVtx == NULL)
        return;

    for (int i = 0; i < min(groundCollParams->line_count, MAX_LINES); i++) {
        CollisionLineDef *def = groundCollLine[i].def;
        Vector2D *from = &groundCollVtx[def->points[0]].pos;
        Vector2D *to = &groundCollVtx[def->points[1]].pos;

        const char *type;
        if (def->type & CollisionLineType_Ledge)
            type = " Ldge";
        else if (def->type & CollisionLineType_Platform)
            type = " Plat";
        else
            type = "";

        menu_lines[i] = (MenuItemLine) {
            .base.type = MenuItem_TextSelectable,
            .base.text = menu_lines[i].buf,
            .line = i
        };

        sprintf(
            menu_lines[i].buf,
            "%2i (%7.03f, %7.03f) to (%7.03f, %7.03f)%s",
            i, from->x, from->y, to->x, to->y, type);

        Menu_AddItem(&collision_info_menu, &menu_lines[i].base);
    }
}

void CollisionInfo_Init(void)
{
	Menu_Init(&collision_info_menu, "Collision Info");

	// Add to main menu
	static MenuItem menu_collision_info = {
		.text = "Collision Info",
		.type = MenuItem_SubMenu,
        .callback = CollisionInfo_Update,
		.u.submenu = &collision_info_menu
	};

	Menu_AddItem(&MainMenu, &menu_collision_info);
}