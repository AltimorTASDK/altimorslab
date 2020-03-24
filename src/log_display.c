#include "melee.h"
#include <stdio.h>

#define TEXT_ID 69
#define TEXT_X 0
#define TEXT_Y 0
#define TEXT_WIDTH 73
#define TEXT_HEIGHT 15
#define TEXT_SCALE_X 9.F
#define TEXT_SCALE_Y 12.F
#define TEXT_FGCOLOR 0x00FF00FF
#define TEXT_BGCOLOR 0x00000040

#define TEXT_DISPLAY_TIME (5*60)

static DevText *log_text;
static char text_buf[TEXT_WIDTH * TEXT_HEIGHT * 2];
static u32 log_hide_time = 0;

static void LogDisplay_Init(void)
{
	log_text = DevelopText_Initialize(
		TEXT_ID, TEXT_X, TEXT_Y, TEXT_WIDTH, TEXT_HEIGHT, text_buf);

	DevelopText_AddToMainList(NULL, log_text);
	DevelopText_ResetUnknownFlag(log_text);
	DevelopText_StoreBGColor(log_text, &(u32) { TEXT_BGCOLOR });
	DevelopText_StoreTextColor(log_text, &(u32) { TEXT_FGCOLOR });
	DevelopText_StoreTextScale(log_text, TEXT_SCALE_X, TEXT_SCALE_Y);

	DevelopText_HideText(log_text);
	DevelopText_HideBackground(log_text);
}

void LogDisplay_Print(const char *message)
{
	DevelopText_Print(log_text, message);
	DevelopText_ShowText(log_text);
	DevelopText_ShowBackground(log_text);
	log_hide_time = MatchInfo_LoadFrameCount() + TEXT_DISPLAY_TIME;
}

void orig_DevelopMode_IngameTogglesMost(void);
void hook_DevelopMode_IngameTogglesMost(void)
{
	orig_DevelopMode_IngameTogglesMost();

	if (MatchInfo_LoadFrameCount() == log_hide_time) {
		DevelopText_Erase(log_text);
		DevelopText_ResetCursorXY(log_text, 0, 0);
		DevelopText_HideText(log_text);
		DevelopText_HideBackground(log_text);
	}
}

void orig_StartMelee(void *param_1);
void hook_StartMelee(void *param_1)
{
	orig_StartMelee(param_1);
	LogDisplay_Init();
}