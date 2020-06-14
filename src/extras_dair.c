#include "hsd.h"
#include "melee.h"
#include "util.h"
#include <math.h>

#define ROTATE_END 5.f
#define REVERT_START 25.f
#define REVERT_END 40.f

#define ROTATION_FAIR deg2rad(-50.f)
#define ROTATION_UAIR deg2rad(-145.f)
#define ROTATION_BAIR deg2rad(115.f)

#define NAIR_KB_ANGLE 361
#define FAIR_KB_ANGLE 25
#define UAIR_KB_ANGLE 90
#define BAIR_KB_ANGLE 361

// Total rotation = NAIR_SPIN_RATE * NAIR_TOTAL_OVER_VEL + NAIR_START_ROTATION
#define NAIR_TOTAL_OVER_VEL (REVERT_START/2 - ROTATE_END/2 + REVERT_END/2)
#define NAIR_START_ROTATION deg2rad(30.f)
#define NAIR_SPIN_RATE ((-6 * M_PI - NAIR_START_ROTATION) / NAIR_TOTAL_OVER_VEL)

static u32 aerial_state[MAX_PLAYERS];

static inline float SmoothStep(float a, float b, float c)
{
	if (c <= 0.f)
		return a;
	if (c >= 1.f)
		return b;

	return a + (b - a) * c*c * (3 - 2 * c);
}

static inline float SmoothStepSum(float a, float b, float c)
{
	// D = B - A
	// A + D * c^2(3 - 2 * c)
	// A + 3Dc^2 - 2Dc^3
	// integral A + 3Dc^2 - 2Dc^3 dc = Ac + Dc^3 - 1/2Dc^4
	// Ac + Dc^3(1 - 1/2c)
	if (c <= 0.f)
		return 0.f;
	if (c >= 1.f)
		return a + (b - a) / 2;

	return a * c + (b - a) * c*c*c * (1 - c / 2);
}

static float AccelAverageSpeed(float frame, float start, float end)
{
	return SmoothStepSum(0.f, end - start, (frame - start) / (end - start));
}

static float DecelAverageSpeed(float frame, float start, float end)
{
	return SmoothStepSum(end - start, 0.f, (frame - start) / (end - start));
}

/*
 * SPIN TO WIN
 */
static float CalculateNairRotation(float frame)
{
	float factor = AccelAverageSpeed(frame, 0.f, ROTATE_END);
	factor += max(min(frame, REVERT_START) - ROTATE_END, 0);
	factor += DecelAverageSpeed(frame, REVERT_START, REVERT_END);
	return factor * NAIR_SPIN_RATE + NAIR_START_ROTATION;
}

static float CalculateRotation(Player *player)
{
	float frame = player->frame_timer;
	u32 aerial = aerial_state[player->slot];

	if (aerial == AS_AttackAirN)
		return CalculateNairRotation(frame);

	float angle =
		aerial == AS_AttackAirF ? ROTATION_FAIR :
		aerial == AS_AttackAirHi ? ROTATION_UAIR :
		aerial == AS_AttackAirB ? ROTATION_BAIR : 0.f;

	float c;
	if (frame < REVERT_START)
		c = frame / ROTATE_END;
	else
		c = 1 - InvLerp(frame, REVERT_START, REVERT_END);

	return SmoothStep(0.f, angle, c);
}

void orig_HSD_JObjAnim(HSD_JObj *jobj);
void hook_HSD_JObjAnim(HSD_JObj *jobj)
{
	orig_HSD_JObjAnim(jobj);

	HSD_GObj *gobj = plinkhigh_gobjs[GOBJ_LINK_PLAYER];
	for (; gobj != NULL; gobj = gobj->next) {
		Player *player = gobj->data;
		u8 slot = player->slot;

		if (slot < 0 || slot >= MAX_PLAYERS)
			continue;

		if (player->character_id != CID_Falco)
			continue;

		if (player->action_state != AS_AttackAirLw)
			continue;

		if (aerial_state[slot] == AS_AttackAirLw)
			continue;

		HSD_JObj *TopN = player->phys.root_bone;
		if (TopN == NULL)
			continue;

		HSD_JObj *TransN = TopN->child;
		if (TransN == NULL)
			continue;

		HSD_JObj *HipN = TransN->child;
		if (jobj == HipN) {
			jobj->rotation.x = CalculateRotation(player);
			break;
		}
	}
}

int orig_Player_MeleeDamage(
	Player *player, Hitbox *hitbox,
	Player *victim, Hurtbox *hurtbox);
int hook_Player_MeleeDamage(
	Player *player, Hitbox *hitbox,
	Player *victim, Hurtbox *hurtbox)
{
	if (player->character_id != CID_Falco)
		return orig_Player_MeleeDamage(player, hitbox, victim, hurtbox);

	if (player->action_state != AS_AttackAirLw)
		return orig_Player_MeleeDamage(player, hitbox, victim, hurtbox);

	u8 slot = player->slot;
	if (slot < 0 || slot >= MAX_PLAYERS)
		return orig_Player_MeleeDamage(player, hitbox, victim, hurtbox);

	u32 aerial = aerial_state[slot];
	if (aerial == AS_AttackAirN)
		hitbox->angle = NAIR_KB_ANGLE;
	else if (aerial == AS_AttackAirF)
		hitbox->angle = FAIR_KB_ANGLE;
	else if (aerial == AS_AttackAirHi)
		hitbox->angle = UAIR_KB_ANGLE;
	else if (aerial == AS_AttackAirB)
		hitbox->angle = BAIR_KB_ANGLE;

	return orig_Player_MeleeDamage(player, hitbox, victim, hurtbox);
}

void ExtrasDair_ASChange(Player *player, u32 *new_state)
{
	if (player->character_id != CID_Falco)
		return;

	if (*new_state < AS_AttackAirN || *new_state > AS_AttackAirLw)
		return;

	u8 slot = player->slot;
	if (slot < 0 || slot >= MAX_PLAYERS)
		return;

	aerial_state[slot] = *new_state;
	*new_state = AS_AttackAirLw;
}