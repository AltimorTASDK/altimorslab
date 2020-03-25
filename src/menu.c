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

static void LogDisplay_Init(void)
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