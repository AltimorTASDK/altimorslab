#include "hsd.h"
#include "melee.h"
#include "util.h"
#include "gx.h"
#include <string.h>

#define B0XX_SCALE .25f

#define B0XX_BUTTON_RADIUS (B0XX_SCALE * 12)
#define B0XX_BUTTON_COLOR_UP 0x7F7F7FFF
#define B0XX_BUTTON_COLOR_DOWN 0xFFFF7FFF
#define B0XX_BUTTON_STROKE 0x0000007F

#define B0XX_L_X (B0XX_SCALE * 46)
#define B0XX_L_Y (B0XX_SCALE * 76)
#define B0XX_LEFT_X (B0XX_SCALE * 71)
#define B0XX_LEFT_Y (B0XX_SCALE * 60)
#define B0XX_DOWN_X (B0XX_SCALE * 100)
#define B0XX_DOWN_Y (B0XX_SCALE * 57)
#define B0XX_RIGHT_X (B0XX_SCALE * 126)
#define B0XX_RIGHT_Y (B0XX_SCALE * 69)

#define B0XX_MX_X (B0XX_SCALE * 133)
#define B0XX_MX_Y (B0XX_SCALE * 131)
#define B0XX_MY_X (B0XX_SCALE * 156)
#define B0XX_MY_Y (B0XX_SCALE * 146)

#define B0XX_R_X (B0XX_SCALE * 279)
#define B0XX_R_Y (B0XX_SCALE * 43)
#define B0XX_B_X (B0XX_SCALE * 279)
#define B0XX_B_Y (B0XX_SCALE * 71)
#define B0XX_Y_X (B0XX_SCALE * 306)
#define B0XX_Y_Y (B0XX_SCALE * 30)
#define B0XX_X_X (B0XX_SCALE * 306)
#define B0XX_X_Y (B0XX_SCALE * 58)
#define B0XX_Z_X (B0XX_SCALE * 334)
#define B0XX_Z_Y (B0XX_SCALE * 62)
#define B0XX_UP_X (B0XX_SCALE * 359)
#define B0XX_UP_Y (B0XX_SCALE * 78)

#define B0XX_CU_X (B0XX_SCALE * 265)
#define B0XX_CU_Y (B0XX_SCALE * 102)
#define B0XX_CL_X (B0XX_SCALE * 241)
#define B0XX_CL_Y (B0XX_SCALE * 117)
#define B0XX_CR_X (B0XX_SCALE * 289)
#define B0XX_CR_Y (B0XX_SCALE * 117)
#define B0XX_CD_X (B0XX_SCALE * 241)
#define B0XX_CD_Y (B0XX_SCALE * 149)
#define B0XX_A_X (B0XX_SCALE * 265)
#define B0XX_A_Y (B0XX_SCALE * 132)

#define B0XX_DISPLAY_HEIGHT (B0XX_SCALE * 500)

#define HISTORY_SIZE 30

// displayed_history isn't updated until a button is pressed
static HSD_PadStatus displayed_history[HISTORY_SIZE] = { 0 };
static HSD_PadStatus history[HISTORY_SIZE] = { 0 };
static int last_input_frame = 0;

static BOOL IsModXHeld(HSD_PadStatus *pad)
{
	float absx = fabs(pad->stick_x);
	float absy = fabs(pad->stick_y);
	return
		(absx == .6375f && absy == .3750f) ||
		(absx == .6625f && absy == .0000f) ||
		(absx == .0000f && absy == .2875f) ||
		(absx == .7375f && absy == .2875f);
}

static BOOL IsModYHeld(HSD_PadStatus *pad)
{
	float absx = fabs(pad->stick_x);
	float absy = fabs(pad->stick_y);
	return
		(absx == .5000f && absy == .8500f) ||
		(absx == .3375f && absy == .0000f) ||
		(absx == .0000f && absy == .7375f) ||
		(absx == .2875f && absy == .7375f);
}

static void DrawB0XXButton(
	float offset_x, float offset_y,
	float button_x, float button_y,
	u32 pressed)
{
	float x = offset_x + button_x;
	float y = offset_y + button_y;
	u32 fill = pressed ? B0XX_BUTTON_COLOR_DOWN : B0XX_BUTTON_COLOR_UP;
	FillCircle2D(x, y, B0XX_BUTTON_RADIUS, 16, fill);
	StrokeCircle2D(x, y, B0XX_BUTTON_RADIUS, 16, B0XX_BUTTON_STROKE);
}

static void DrawPad(HSD_PadStatus *pad, float x, float y)
{
	DrawB0XXButton(x, y, B0XX_L_X, B0XX_L_Y, pad->buttons & Button_L);
	DrawB0XXButton(x, y, B0XX_LEFT_X, B0XX_LEFT_Y, pad->stick_x < 0);
	DrawB0XXButton(x, y, B0XX_DOWN_X, B0XX_DOWN_Y, pad->stick_y < 0);
	DrawB0XXButton(x, y, B0XX_RIGHT_X, B0XX_RIGHT_Y, pad->stick_x > 0);

	DrawB0XXButton(x, y, B0XX_MX_X, B0XX_MX_Y, IsModXHeld(pad));
	DrawB0XXButton(x, y, B0XX_MY_X, B0XX_MY_Y, IsModYHeld(pad));

	DrawB0XXButton(x, y, B0XX_R_X, B0XX_R_Y, pad->buttons & Button_R);
	DrawB0XXButton(x, y, B0XX_B_X, B0XX_B_Y, pad->buttons & Button_B);
	DrawB0XXButton(x, y, B0XX_Y_X, B0XX_Y_Y, pad->buttons & Button_Y);
	DrawB0XXButton(x, y, B0XX_X_X, B0XX_X_Y, pad->buttons & Button_X);
	DrawB0XXButton(x, y, B0XX_Z_X, B0XX_Z_Y, pad->buttons & Button_Z);
	DrawB0XXButton(x, y, B0XX_UP_X, B0XX_UP_Y, pad->stick_y > 0);

	DrawB0XXButton(x, y, B0XX_CU_X, B0XX_CU_Y, pad->cstick_y > 0);
	DrawB0XXButton(x, y, B0XX_CL_X, B0XX_CL_Y, pad->cstick_x < 0);
	DrawB0XXButton(x, y, B0XX_CR_X, B0XX_CR_Y, pad->cstick_x > 0);
	DrawB0XXButton(x, y, B0XX_CD_X, B0XX_CD_Y, pad->cstick_y < 0);
	DrawB0XXButton(x, y, B0XX_A_X, B0XX_A_Y, pad->buttons & Button_A);
}

static BOOL IsPadNonZero(HSD_PadStatus *pad)
{
	return
		pad->buttons != 0 ||
		pad->stick_x != 0 || pad->stick_y != 0 ||
		pad->cstick_x != 0 || pad->cstick_y != 0 ||
		pad->analog_l != 0 || pad->analog_r != 0;
}

static HSD_PadStatus *GetHistory(u32 frame)
{
	return &history[0];
}

static HSD_PadStatus *GetDisplayedHistory(u32 frame)
{
	return &displayed_history[frame % HISTORY_SIZE];
}

static void UpdateDisplay(int port)
{
	HSD_PadStatus *pad = &HSD_PadMasterStatus[port];
	u32 current_frame = MatchInfo_LoadFrameCount();

	memcpy(GetHistory(current_frame), pad, sizeof(HSD_PadStatus));
	if (IsPadNonZero(pad)) {
		last_input_frame = current_frame;
		memcpy(displayed_history, history, sizeof(history));
	}

	for (int i = 0; i < min(current_frame, HISTORY_SIZE); i++) {
		u32 frame = last_input_frame - i;
		float offset = (HISTORY_SIZE - i - 1) * B0XX_DISPLAY_HEIGHT;
		DrawPad(GetDisplayedHistory(frame), 0.f, offset);
	}
}

void InputDisplay_Draw(RenderPass pass)
{
	if (pass != RenderPass_Translucent)
		return;

	HSD_StateInitDirect(GX_VTXFMT0, 2);
	GXSetLineWidth(2, GX_TO_ONE);
	UpdateDisplay(0);
}