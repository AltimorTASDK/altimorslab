#include "int_types.h"
#include <stdio.h>

typedef struct _DevText {
	char pad[0x2C];
} DevText;

typedef struct _Player {
	char pad0000[0x10];
	u32 action_state;
	char pad0014[0x894 - 0x14];
	float frame_timer;
} Player;

typedef enum ActionState_ {
	AS_NAMED_MAX = 0x155
} ActionState;

extern const char *action_state_names[AS_NAMED_MAX];

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