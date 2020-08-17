#include "melee.h"

#define TEXT_ID 69
#define TEXT_X 0
#define TEXT_Y 0
#define TEXT_WIDTH 73
#define TEXT_HEIGHT 15
#define TEXT_SCALE_X 9.F
#define TEXT_SCALE_Y 12.F
#define TEXT_COLOR0 0x00FF00FF
#define TEXT_COLOR1 0xFF0000FF
#define TEXT_COLOR2 0x007FFFFF
#define TEXT_COLOR3 0xFFFFFFFF
#define TEXT_BGCOLOR 0x00000040

#define TEXT_DISPLAY_TIME (5*60)

static DevText *log_text;
static char text_buf[TEXT_WIDTH * TEXT_HEIGHT * 2];
static u32 log_hide_time = 0;

void LogDisplay_CreateText(void)
{
	log_text = DevelopText_Initialize(
		TEXT_ID, TEXT_X, TEXT_Y, TEXT_WIDTH, TEXT_HEIGHT, text_buf);

	DevelopText_AddToMainList(NULL, log_text);
	DevelopText_ResetUnknownFlag(log_text);
	DevelopText_StoreBGColor(log_text, &(u32) { TEXT_BGCOLOR });
	DevelopText_StoreTextScale(log_text, TEXT_SCALE_X, TEXT_SCALE_Y);

	DevelopText_StoreColorIndex(log_text, 3);
	DevelopText_StoreTextColor(log_text, &(u32) { TEXT_COLOR3 });
	DevelopText_StoreColorIndex(log_text, 2);
	DevelopText_StoreTextColor(log_text, &(u32) { TEXT_COLOR2 });
	DevelopText_StoreColorIndex(log_text, 1);
	DevelopText_StoreTextColor(log_text, &(u32) { TEXT_COLOR1 });
	DevelopText_StoreColorIndex(log_text, 0);
	DevelopText_StoreTextColor(log_text, &(u32) { TEXT_COLOR0 });

	DevelopText_HideText(log_text);
	DevelopText_HideBackground(log_text);
}

void LogDisplay_Update(void)
{
	if (MatchInfo_LoadFrameCount() == log_hide_time) {
		DevelopText_Erase(log_text);
		DevelopText_SetCursorXY(log_text, 0, 0);
		DevelopText_HideText(log_text);
		DevelopText_HideBackground(log_text);
	}
}

void LogDisplay_Print(const char *message)
{
	DevelopText_ColorPrint(log_text, message);
	DevelopText_ShowText(log_text);
	DevelopText_ShowBackground(log_text);
	log_hide_time = MatchInfo_LoadFrameCount() + TEXT_DISPLAY_TIME;
}