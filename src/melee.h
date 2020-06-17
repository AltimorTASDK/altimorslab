#pragma once

#include "int_types.h"
#include "hsd.h"
#include <stdio.h>

#define DEADZONE .2875F
#define MAX_PLAYERS 6
#define MAX_HITBOXES 4
#define MAX_HURTBOXES 15

#define DevelopText_ColorPrintf(text, format, ...) \
{ \
	char buf[256]; \
	sprintf(buf, format, __VA_ARGS__); \
	DevelopText_ColorPrint(text, buf); \
}

#define DevelopText_BigBufPrintf(text, format, ...) \
{ \
	char buf[256]; \
	sprintf(buf, format, __VA_ARGS__); \
	DevelopText_Print(text, buf); \
}

struct _Text;
typedef struct _Text Text;

typedef enum _ActionState {
	AS_Wait = 0xE,
	AS_DamageFall = 0x26,
	AS_DamageFlyHi = 0x57,
	AS_DamageFlyN = 0x58,
	AS_DamageFlyLw = 0x59,
	AS_DamageFlyTop = 0x5A,
	AS_DamageFlyRoll = 0x5B,
	AS_AttackAirN = 0x41,
	AS_AttackAirF = 0x42,
	AS_AttackAirB = 0x43,
	AS_AttackAirHi = 0x44,
	AS_AttackAirLw = 0x45,
	AS_CliffCatch = 0xFC,
	AS_NAMED_MAX = 0x155
} ActionState;

typedef enum _DebugLevel {
	DbLevel_Master,
	DbLevel_NoDebugRom,
	DbLevel_DebugDevelop,
	DbLevel_DebugRom,
	DbLevel_Develop
} DebugLevel;

typedef enum _CID {
	CID_Falco = 0x16
} CID;

typedef enum _OverlayFlag {
	OverlayFlag_UseColor = 0x80
} OverlayFlag;

typedef enum _RenderFlag {
	RenderFlag_OverrideColor = 0x20
} RenderFlag;

typedef enum _BodyState {
	BodyState_Normal = 0,
	BodyState_Invincible = 1,
	BodyState_Intangible = 2
} BodyState;

typedef enum _HitboxState {
	HitboxState_Interpolated = 1,
	HitboxState_Activated = 2
} HitboxState;

typedef enum _HitboxFlag4 {
	HitboxFlag4_NoScaling = 0x40
} HitboxFlag4;

typedef enum _HurtboxFlag {
	HurtboxFlag_PositionUpdated = 0x80
} HurtboxFlag;

typedef enum _ASChangeFlag {
	ASChangeFlag_KeepFastFallState = 0x1,
	ASChangeFlag_KeepBodyState = 0x4,
	ASChangeFlag_KeepHitboxes = 0x8,
	ASChangeFlag_KeepDObjFlags = 0x10,
	ASChangeFlag_NoAnimationVelocity = 0x20,
	ASChangeFlag_NoAnimationReset = 0x80,
	ASChangeFlag_SkipSubactionEvents = 0x4000,
	ASChangeFlag_NoSubaction = 0x20000000
} ASChangeFlag;

typedef struct _DevText {
	u16 x, y;
	u8 w, h;
	u8 cursor_x, cursor_y;
	float scale_x, scale_y;
	u32 bg_color;
	u32 text_colors[4];
	u8 id;
	u8 line_width;
	u8 flags;
	u8 current_color;
	char *buf;
	struct _DevText *prev;
	struct _DevText *next;
} DevText;

typedef struct _Hitbox {
	u32 state;
	u32 active;
	u32 damage;
	float staled_damage;
	Vector offset;
	float radius;
	u32 angle;
	u32 kbg;
	u32 fkv;
	u32 bkb;
	u32 element;
	u32 shield_damage;
	char pad0038[0x3C - 0x38];
	u32 sfx;
	u8 flags1;
	u8 flags2;
	u8 flags3;
	u8 flags4;
	char pad0044[0x4C - 0x44];
	Vector position;
	Vector last_position;
	Vector last_contact;
	float last_contact_depth;
	struct {
		HSD_GObj *gobj;
		u32 flags;
	} hit_objects[12];
	char pad00D4[0x138 - 0xD4];
} Hitbox;

typedef struct _Hurtbox {
	u32 body_state;
	Vector offset1;
	Vector offset2;
	float radius;
	HSD_JObj *bone;
	u8 flags;
	char align0025[0x28 - 0x25];
	Vector position1;
	Vector position2;
	u32 bone_id;
	u32 flinch_type;
	u32 grabbable;
} Hurtbox;

typedef struct _ECB {
	Vector2D top;
	Vector2D bottom;
	Vector2D right;
	Vector2D left;
} ECB;

typedef struct _Physics {
	HSD_GObj *gobj;
	Vector position;
	Vector trace_start_position;
	Vector start_position;
	Vector last_position;
	char pad0034[0x84 - 0x34];
	ECB desired_ecb;
	ECB ecb;
	ECB trace_start_ecb;
	ECB last_ecb;
	int use_ecb_bones;
	HSD_JObj *root_bone;
	HSD_JObj *ecb_bones[6];
	char pad0124[0x130 - 0x124];
	u32 ecb_update_flags;
	u32 surface_type;
	u32 last_surface_type;
	char pad013C[0x19C - 0x13C];
	int ecb_timer;
} Physics;

typedef struct _PlayerInput {
    float stick_x;
    float stick_y;
    float last_stick_x;
    float last_stick_y;
    float frozen_stick_x;
    float frozen_stick_y;
    float cstick_x;
    float cstick_y;
    float last_cstick_x;
    float last_cstick_y;
    float frozen_cstick_x;
    float frozen_cstick_y;
    float analog_lr;
    float last_analog_lr;
    float frozen_analog_lr;
    u32 held_buttons;
    u32 last_held_buttons;
    u32 frozen_buttons;
    u32 instant_buttons;
    u32 released_buttons;
    u8 stick_x_hold_time;
    u8 stick_y_hold_time;
    u8 analog_lr_hold_time;
    u8 stick_x_hold_time_2;
    u8 stick_y_hold_time_2;
    u8 analog_lr_hold_time_2;
    u8 stick_x_neutral_time;
    u8 stick_y_neutral_time;
    u8 analog_lr_neutral_time;
    u8 stick_x_direction_hold_time;
    u8 stick_y_direction_hold_time;
    u8 analog_lr_direction_hold_time;
    u8 last_a_press;
    u8 last_b_press;
    u8 last_xy_press;
    u8 last_analog_lr_press;
    u8 last_digital_lr_press;
    u8 last_dpad_down_press;
    u8 last_dpad_up_press;
    u8 a_press_interval;
    u8 digital_lr_press_interval;
    u8 last_jump_input;
    u8 last_up_b_input;
    u8 last_down_b_input;
    u8 last_side_b_input;
    u8 last_neutral_b_input;
    u8 jump_input_interval;
    u8 up_b_input_interval;
} PlayerInput;

typedef struct _SkeletonInfo {
	char pad0000[0x04];
	u8 *common_bone_mapping;
	u32 bone_count;
} SkeletonInfo;

typedef struct _PlayerBone {
	HSD_JObj *jobj;
	HSD_JObj *lerp_target_jobj;
	u8 flags;
	char pad0009[0x10 - 0x09];
} PlayerBone;

typedef struct _Player {
	HSD_GObj *gobj;
	u32 character_id;
	u32 spawn_count;
	u8 slot;
	char align000D[0x10 - 0x0D];
	u32 action_state;
	char pad0014[0x2C - 0x14];
	float direction;
	float last_direction;
	float initial_scale;
	float scale;
	char pad003C[0xB0 - 0x3C];
	Vector position;
	Vector last_position;
	char pad00C8[0xE0 - 0xC8];
	u32 airborne;
	char pad00E4[0x4B8 - 0xE4];
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
	char pad0505[0x614 - 0x505];
	u8 override_color_r;
	u8 override_color_g;
	u8 override_color_b;
	u8 override_color_a;
	char pad0618[0x620 - 0x618];
	PlayerInput input;
	char pad068C[0x6F0 - 0x68C];
	Physics phys;
	void *camera_data;
	float frame_timer;
	char pad0898[0x914 - 0x898];
	Hitbox hitboxes[MAX_HITBOXES];
	char pad0DF4[0x119E - 0xDF4];
	u8 hurtbox_count;
	char pad119F[0x11A0 - 0x119F];
	Hurtbox hurtboxes[MAX_HURTBOXES];
	char pad1614[0x1988 - 0x1614];
	u32 body_state_subaction;
	u32 body_state_timed;
	s32 intangible_frames;
	s32 invincible_frames;
	float shield_size;
	float lightshield_amount;
	s32 hit_shield_damage;
	char pad19A4[0x2223 - 0x19A4];
	u8 render_flags;
} Player;

extern u32 DbLevel;
extern Text *NameTagText;

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

// Camera
int WorldToScreen(Vector *in, Vector2D *out);
HSD_GObj *Camera_LoadCameraEntity(void);

// Text
int Text_InitializeSubtext(Text *text, float x, float y, const char *fmt, ...);
void Text_UpdateSubtextContents(Text *text, u32 subtext, const char *fmt, ...);
void Text_UpdateSubtextPosition(Text *text, u32 subtext, float x, float y);
void Text_UpdateSubtextSize(Text *text, u32 subtext, float x, float y);
void Text_ChangeSubtextColor(Text *text, u32 subtext, u32 *color);

// Player
int Player_IsCPU(Player *player);
int Player_IsKnockdownFaceUp(HSD_GObj *gobj);
void Hurtbox_Update(Hurtbox *hurtbox);

// Developer text
DevText *DevelopText_Initialize(
	u8 id, u16 x, u16 y, u16 width, u16 height, char *buf);
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
void DevelopText_ResetCursorXY(DevText *text, u16 x, u16 y);
void DevelopText_StoreColorIndex(DevText *text, u8 index);

/*
 * Print a dev text message using \x01 through \x04 as color codes
 */
void DevelopText_ColorPrint(DevText *text, const char *message);