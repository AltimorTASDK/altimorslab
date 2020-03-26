#pragma once

#include "int_types.h"
#include <stdio.h>

#define DEADZONE .2875F

#define DevelopText_ColorPrintf(format, ...) \
	{ \
		char buf[256]; \
		sprintf(buf, format, __VA_ARGS__); \
		DevelopText_ColorPrint(buf); \
	}

typedef enum ActionState_ {
	AS_Wait = 0xE,
	AS_NAMED_MAX = 0x155
} ActionState;

typedef enum DebugLevel_ {
	DbLevel_Master,
	DbLevel_NoDebugRom,
	DbLevel_DebugDevelop,
	DbLevel_DebugRom,
	DbLevel_Develop
} DebugLevel;

typedef enum Button_ {
	Button_DPadLeft = 1,
	Button_DPadRight = 2,
	Button_DPadDown = 4,
	Button_DPadUp = 8,
	Button_Z = 0x10,
	Button_A = 0x100,
	Button_B = 0x200,
	Button_X = 0x400,
	Button_Y = 0x800,
	Button_Start = 0x1000,
	Button_AnalogLR = 0x80000000
} Button;

typedef enum OverlayFlag_ {
	OverlayFlag_UseColor = 0x80
} OverlayFlag;

typedef struct _DevText {
	char pad[0x2C];
} DevText;

typedef struct _HSD_PadStatus {
	Button buttons;
	Button last_buttons;
	Button instant_buttons;
	Button repeated_buttons;
	Button released_buttons;
	s32 repeat_count;
	s8 raw_stick_x;
	s8 raw_stick_y;
	s8 raw_cstick_x;
	s8 raw_cstick_y;
	u8 raw_analog_l;
	u8 raw_analog_r;
	u8 raw_analog_a;
	u8 raw_analog_b;
	float stick_x;
	float stick_y;
	float cstick_x;
	float cstick_y;
	float analog_l;
	float analog_r;
	float analog_a;
	float analog_b;
	u8 cross_dir;
	u8 supports_rumble;
	s8 err;
} HSD_PadStatus;

typedef struct _Player {
	char pad0000[0x10];
	u32 action_state;
	char pad0014[0x4B8 - 0x14];
	float overlay_r;
	float overlay_g;
	float overlay_b;
	float overlay_a;
	float overlay_flash_rate_r;
	float overlay_flash_rate_g;
	float overlay_flash_rate_b;
	float overlay_flash_rate_a;
	char pad04D8[0x504 - 0x4D8];
	u8 overlay_flags;
	char pad0505[0x894 - 0x505];
	float frame_timer;
} Player;

extern HSD_PadStatus HSD_PadMasterStatus[4];
extern u32 DbLevel;

extern const char *action_state_names[AS_NAMED_MAX];
extern const char *debug_level_names[5];

static inline void GetActionStateName(u32 state, char *buf)
{
	if (state < AS_NAMED_MAX)
		sprintf(buf, "%s", action_state_names[state]);
	else
		sprintf(buf, "%03d", state);
}

// Entry point
void start(void);

// Match info
u32 MatchInfo_LoadFrameCount(void);

// Developer text
DevText *DevelopText_Initialize(
	char id, short x, short y, int width, int height, char *buf);
void DevelopText_AddToMainList(void *unused, DevText *text);
void DevelopText_ResetUnknownFlag(DevText *text);
void DevelopText_StoreBGColor(DevText *text, u32 *color);
void DevelopText_StoreTextColor(DevText *text, u32 *color);
void DevelopText_StoreTextScale(DevText *text, float x, float y);
void DevelopText_HideText(DevText *text);
void DevelopText_ShowText(DevText *text);
void DevelopText_HideBackground(DevText *text);
void DevelopText_ShowBackground(DevText *text);
void DevelopText_Print(DevText *text, const char *message);
void DevelopText_Printf(DevText *text, const char *format, ...);
void DevelopText_Erase(DevText *text);
void DevelopText_ResetCursorXY(DevText *text, int x, int y);
void DevelopText_StoreColorIndex(DevText *text, int index);

/*
 * Print a dev text message using \x01 through \x04 as color codes
 */
void DevelopText_ColorPrint(DevText *text, const char *message);