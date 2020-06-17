#include "hsd.h"
#include "melee.h"
#include "menu.h"

static Menu cpu_control_menu;
static int force_shield;
static int freeze_position;

void CPUControl_UpdateInput(Player *player)
{
    if (force_shield && Player_IsCPU(player)) {
		player->input.analog_lr = 1.f;
		player->input.held_buttons |= Button_AnalogLR;
		player->shield_size = 60.f;
	}
}

void CPUControl_PhysicsMove(Physics *phys)
{
	if (!freeze_position || phys->gobj->classifier != GOBJ_CLASS_PLAYER)
		return;

	if (Player_IsCPU(phys->gobj->data)) {
		phys->position = phys->start_position;
		phys->desired_ecb = phys->ecb;
	}
}

BOOL CPUControl_ASChange(Player *player)
{
	return !freeze_position || !Player_IsCPU(player);
}

void CPUControl_Init(void)
{
	static MenuItem menu_force_shield = {
		.text = "Force Shield",
		.type = MenuItem_AdjustEnum,
		.u = {
			.adjust_enum = {
				&force_shield,
				1, 0, 1, TRUE,
				on_off_text
			}
		}
	};

	static MenuItem menu_freeze_position = {
		.text = "Freeze Position",
		.type = MenuItem_AdjustEnum,
		.u = {
			.adjust_enum = {
				&freeze_position,
				1, 0, 1, TRUE,
				on_off_text
			}
		}
	};

	Menu_Init(&cpu_control_menu, "CPU Control");
	Menu_AddItem(&cpu_control_menu, &menu_force_shield);
	Menu_AddItem(&cpu_control_menu, &menu_freeze_position);

	// Add to main menu
	static MenuItem menu_cpu_control = {
		.text = "CPU Control",
		.type = MenuItem_SubMenu,
		.u = { .submenu = &cpu_control_menu }
	};

	Menu_AddItem(&main_menu, &menu_cpu_control);

}