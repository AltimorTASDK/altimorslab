#include "util.h"
#include "melee.h"
#include "menu.h"
#include <math.h>

#define TEXT_SIZE .3f
#define TEXT_MARGIN 2.f
#define CHAR_WIDTH (20.f * TEXT_SIZE)
#define DASH_WIDTH (CHAR_WIDTH * 2)
#define TEXT_HEIGHT (28.f * TEXT_SIZE)
#define TEXT_Y_OFFSET (-90.f * TEXT_SIZE)

#define HITBOX_COLOR 0xFF0000FF
#define HURTBOX_COLOR 0xFFFF00FF
#define DISJOINT_COLOR 0xFFFFFFFF
#define SHADOW_COLOR 0x000000FF

#define PUSH_ITERATIONS 10
// Enough for the deepest intersection to be resolved in PUSH_ITERATIONS
#define MAX_PUSH ((CHAR_WIDTH * 5 + DASH_WIDTH) / PUSH_ITERATIONS / 2)

typedef struct _ShadowText {
	u32 fg, bg;
} ShadowText;

typedef struct _MinMaxText {
	ShadowText min_x, min_y;
	ShadowText max_x, max_y;
} MinMaxText;

typedef union _MinMaxPositions {
	struct {
		Vector min_x, min_y;
		Vector max_x, max_y;
		float min_x_width, min_y_width;
		float max_x_width, max_y_width;
		Vector min_x_push, min_y_push;
		Vector max_x_push, max_y_push;
	};
	struct {
		Vector points[4];
		float widths[4];
		Vector push[4];
	};
} MinMaxPositions;

static Menu hitbox_stats_menu;
static BOOL initialized[MAX_PLAYERS];
static MinMaxText hitbox_text[MAX_PLAYERS];
static MinMaxText hurtbox_text[MAX_PLAYERS];
static MinMaxText disjoint_text[MAX_PLAYERS];
static int hitbox_text_enabled;
static int hurtbox_text_enabled;
static int disjoint_text_enabled;

static void MinMaxText_InitSubtext(ShadowText *text, u32 color)
{
	text->bg = Text_InitializeSubtext(NameTagText, -5000, 0, "");
	text->fg = Text_InitializeSubtext(NameTagText, -5000, 0, "");
	Text_UpdateSubtextSize(NameTagText, text->bg, TEXT_SIZE, TEXT_SIZE);
	Text_UpdateSubtextSize(NameTagText, text->fg, TEXT_SIZE, TEXT_SIZE);
	Text_ChangeSubtextColor(NameTagText, text->bg, &(u32) { SHADOW_COLOR });
	Text_ChangeSubtextColor(NameTagText, text->fg, &color);
}

static void MinMaxText_Initialize(MinMaxText *text, u32 color)
{
	MinMaxText_InitSubtext(&text->min_x, color);
	MinMaxText_InitSubtext(&text->min_y, color);
	MinMaxText_InitSubtext(&text->max_x, color);
	MinMaxText_InitSubtext(&text->max_y, color);
}

static BOOL GetHitboxBounds(Player *player, Vector *min, Vector *max)
{
	int checked = 0;

	for (int i = 0; i < MAX_HITBOXES; i++) {
		Hitbox *hitbox = &player->hitboxes[i];
		if (hitbox->state == 0)
			continue;

		Vector pos1, pos2;
		PSVECSubtract(&hitbox->position, &player->position, &pos1);

		if (hitbox->state & HitboxState_Interpolated) {
			// Compensate for character movement
			PSVECSubtract(
				&hitbox->last_position,
				&player->last_position,
				&pos2);
		} else {
			pos2 = pos1;
		}

		float r;
		if (hitbox->flags4 & HitboxFlag4_NoScaling)
			r = hitbox->radius;
		else
			r = hitbox->radius * player->scale;

		Vector hitbox_min, hitbox_max;
		VectorMin(&pos1, &pos2, &hitbox_min);
		VectorMax(&pos1, &pos2, &hitbox_max);
		PSVECAdd(&hitbox_min, &(Vector) { -r, -r, -r }, &hitbox_min);
		PSVECAdd(&hitbox_max, &(Vector) { r, r, r }, &hitbox_max);

		if (checked++ == 0) {
			// Initialize
			*min = hitbox_min;
			*max = hitbox_max;
		} else {
			VectorMin(&hitbox_min, min, min);
			VectorMax(&hitbox_max, max, max);
		}
	}

	return checked != 0;
}

static BOOL GetHurtboxBounds(Player *player, Vector *min, Vector *max)
{
	int checked = 0;

	for (int i = 0; i < player->hurtbox_count; i++) {
		Hurtbox *hurtbox = &player->hurtboxes[i];
		if (hurtbox->body_state == BodyState_Intangible)
			continue;

		// Ensure worldspace positions are up to date
		Hurtbox_Update(hurtbox);

		Vector pos1, pos2;
		PSVECSubtract(&hurtbox->position1, &player->position, &pos1);
		PSVECSubtract(&hurtbox->position2, &player->position, &pos2);

		float r = hurtbox->radius;
		Vector hurtbox_min, hurtbox_max;
		VectorMin(&pos1, &pos2, &hurtbox_min);
		VectorMax(&pos1, &pos2, &hurtbox_max);
		PSVECAdd(&hurtbox_min, &(Vector) { -r, -r, -r }, &hurtbox_min);
		PSVECAdd(&hurtbox_max, &(Vector) { r, r, r }, &hurtbox_max);

		if (checked++ == 0) {
			// Initialize
			*min = hurtbox_min;
			*max = hurtbox_max;
		} else {
			VectorMin(&hurtbox_min, min, min);
			VectorMax(&hurtbox_max, max, max);
		}
	}

	return checked != 0;
}

static void PushPointsApart(
	Vector *a,
	Vector *b,
	float width,
	float height,
	Vector *push_a,
	Vector *push_b)
{
	if (a->x == b->x && a->y == b->y) {
		// Avoid div 0
		push_a->y += height / 2;
		push_b->y -= height / 2;
		return;
	}

	// Treat the boundaries of each point as an ellipse
	float dist_x = a->x - b->x;
	float dist_y = a->y - b->y;
	float dist_x_sqr = dist_x * dist_x;
	float dist_y_sqr = dist_y * dist_y;
	float width_sqr = width * width;
	float height_sqr = height * height;

	if (dist_x_sqr / width_sqr + dist_y_sqr / height_sqr >= 1)
		return;

	// Find the distance to the edge of the ellipse
	float dir_x_sqr = dist_x_sqr / (dist_x_sqr + dist_y_sqr);
	float dir_y_sqr = dist_y_sqr / (dist_x_sqr + dist_y_sqr);
	float new_dist_sqr =
		width_sqr * height_sqr /
		(dir_x_sqr * height_sqr + dir_y_sqr * width_sqr);

	float new_dist_x = sqrtf(dir_x_sqr * new_dist_sqr);
	float new_dist_y = sqrtf(dir_y_sqr * new_dist_sqr);
	float delta_x = new_dist_x * (dist_x > 0 ? 1 : -1) - dist_x;
	float delta_y = new_dist_y * (dist_y > 0 ? 1 : -1) - dist_y;

	push_a->x += delta_x / 2;
	push_a->y += delta_y / 2;
	push_b->x -= delta_x / 2;
	push_b->y -= delta_y / 2;
}

static void PushMinMaxPositionApart(MinMaxPositions *a, MinMaxPositions *b)
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (&a->points[i] == &b->points[j])
				continue;

			float width = (a->widths[i] + b->widths[j]) / 2;
			PushPointsApart(
				&a->points[i], &b->points[j],
				width + TEXT_MARGIN, TEXT_HEIGHT + TEXT_MARGIN,
				&a->push[i], &b->push[j]);
		}
	}
}

static BOOL PreventMinMaxOverlap(
	MinMaxPositions *positions,
	int count,
	float max_push)
{
	for (int i = 0; i < count; i++) {
		for (int j = 0; j < 4; j++)
			positions[i].push[j] = (Vector) { 0, 0, 0 };
	}

	// Get push amounts
	for (int i = 0; i < count; i++) {
		for (int j = 0; j < count; j++) {
			PushMinMaxPositionApart(&positions[i], &positions[j]);
		}
	}

	// Apply push
	BOOL pushed = FALSE;

	for (int i = 0; i < count; i++) {
		for (int j = 0; j < 4; j++) {
			Vector *point = &positions[i].points[j];
			Vector *push = &positions[i].push[j];

			if (push->x == 0 && push->y == 0 && push->z == 0)
				continue;

			float push_amount = PSVECMag(push);
			if (push_amount > max_push)
				PSVECScale(push, max_push / push_amount, push);

			PSVECAdd(point, push, point);
			pushed = TRUE;
		}
	}

	return pushed;
}

static void MinMaxPositionsToScreen(
	MinMaxPositions *in,
	int count,
	MinMaxPositions *out)
{
	HSD_GObj *camera = Camera_LoadCameraEntity();
	HSD_CObj *cobj = (HSD_CObj*)camera->hsd_obj;

	for (int i = 0; i < count; i++) {
		// 3D to 2D
		Vector offsets[] = {
			{ -in[i].min_x_width / 2, TEXT_Y_OFFSET, 0 },
			{ 0, TEXT_Y_OFFSET + TEXT_HEIGHT / 2, 0 },
			{ in[i].max_x_width / 2, TEXT_Y_OFFSET, 0 },
			{ 0, TEXT_Y_OFFSET - TEXT_HEIGHT / 2, 0 }
		};

		for (int j = 0; j < 4; j++) {
			Vector *in_point = &in[i].points[j];
			Vector *out_point = &out[i].points[j];
			HSD_CObjTransform(cobj, in_point, out_point, 0);
			PSVECAdd(out_point, &offsets[j], out_point);
		}
	}

	for (int i = 0; i < PUSH_ITERATIONS; i++) {
		if (!PreventMinMaxOverlap(out, count, MAX_PUSH))
			break;
	}
}

static float GetTextWidth(float value)
{
	// Width of 0.00 + another digit for each power of 10
	float width = CHAR_WIDTH * (4 + (int)(logf(fabs(value)) / LOG10));
	// Add dash for negatives
	return value >= 0 ? width : width + DASH_WIDTH;
}

static void GetMinMaxPositions(
	Vector *origin,
	Vector *mins,
	Vector *maxes,
	Vector *min_pos,
	Vector *max_pos,
	int count,
	MinMaxPositions *out)
{
	for (int i = 0; i < count; i++) {
		Vector world_min, world_max;
		PSVECAdd(&mins[i], origin, &world_min);
		PSVECAdd(&maxes[i], origin, &world_max);

		Vector center;
		VectorAverage(&min_pos[i], &max_pos[i], &center);

		out[i].min_x = (Vector) { min_pos[i].x, center.y, center.z };
		out[i].max_x = (Vector) { max_pos[i].x, center.y, center.z };
		out[i].min_y = (Vector) { center.x, min_pos[i].y, center.z };
		out[i].max_y = (Vector) { center.x, max_pos[i].y, center.z };
		out[i].min_x_width = GetTextWidth(mins[i].x);
		out[i].min_y_width = GetTextWidth(mins[i].y);
		out[i].max_x_width = GetTextWidth(maxes[i].x);
		out[i].max_y_width = GetTextWidth(maxes[i].y);
	}

	MinMaxPositionsToScreen(out, count, out);
}

static void ShadowText_UpdatePosition(ShadowText *text, float x, float y)
{
	Text_UpdateSubtextPosition(NameTagText, text->fg, x, y);
	Text_UpdateSubtextPosition(NameTagText, text->bg, x + 1, y + 1);
}

static void MinMaxText_Hide(MinMaxText *text)
{
	ShadowText_UpdatePosition(&text->min_x, -5000, 0);
	ShadowText_UpdatePosition(&text->max_x, -5000, 0);
	ShadowText_UpdatePosition(&text->min_y, -5000, 0);
	ShadowText_UpdatePosition(&text->max_y, -5000, 0);
}

static void MinMaxText_UpdateComponent(
	ShadowText *text,
	Vector position,
	float value)
{
	ShadowText_UpdatePosition(text, position.x, position.y);
	Text_UpdateSubtextContents(NameTagText, text->fg, "%.2f", value);
	Text_UpdateSubtextContents(NameTagText, text->bg, "%.2f", value);
}

static void MinMaxText_Update(
	MinMaxText *text,
	MinMaxPositions *positions,
	Vector *min,
	Vector *max)
{
	MinMaxText_UpdateComponent(&text->min_x, positions->min_x, min->x);
	MinMaxText_UpdateComponent(&text->max_x, positions->max_x, max->x);
	MinMaxText_UpdateComponent(&text->min_y, positions->min_y, min->y);
	MinMaxText_UpdateComponent(&text->max_y, positions->max_y, max->y);
}

static void InitializeSlot(u8 slot)
{
	MinMaxText_Initialize(&hitbox_text[slot], HITBOX_COLOR);
	MinMaxText_Initialize(&hurtbox_text[slot], HURTBOX_COLOR);
	MinMaxText_Initialize(&disjoint_text[slot], DISJOINT_COLOR);
}

void HitboxStats_UpdatePlayer(Player *player)
{
	if (player->flags2 & PlayerFlag2_Dormant)
		return;

	u8 slot = player->slot;
	if (slot < 0 || slot >= MAX_PLAYERS)
		return;

	// Create subtext for this player if it doesn't exist
	if (!initialized[slot]) {
		InitializeSlot(slot);
		initialized[slot] = TRUE;
	}

	Vector hit_min, hit_max;
	BOOL has_hitboxes = GetHitboxBounds(player, &hit_min, &hit_max);

	Vector hurt_min, hurt_max;
	BOOL has_hurtboxes = GetHurtboxBounds(player, &hurt_min, &hurt_max);

	int count = 0;
	MinMaxText *texts[3];
	Vector mins[3], maxes[3];
	Vector min_pos[3], max_pos[3];

	Vector hit_min_pos, hit_max_pos;
	if (has_hitboxes) {
		PSVECAdd(&hit_min, &player->position, &hit_min_pos);
		PSVECAdd(&hit_max, &player->position, &hit_max_pos);
		if (hitbox_text_enabled) {
			PSVECScale(&hit_min, -1, &mins[count]);
			maxes[count] = hit_max;
			min_pos[count] = hit_min_pos;
			max_pos[count] = hit_max_pos;
			texts[count++] = &hitbox_text[slot];
		}
	}

	Vector hurt_min_pos, hurt_max_pos;
	if (has_hurtboxes) {
		PSVECAdd(&hurt_min, &player->position, &hurt_min_pos);
		PSVECAdd(&hurt_max, &player->position, &hurt_max_pos);
		if (hurtbox_text_enabled) {
			PSVECScale(&hurt_min, -1, &mins[count]);
			maxes[count] = hurt_max;
			min_pos[count] = hurt_min_pos;
			max_pos[count] = hurt_max_pos;
			texts[count++] = &hurtbox_text[slot];
		}
	}

	if (has_hitboxes && has_hurtboxes && disjoint_text_enabled) {
		PSVECSubtract(&hurt_min, &hit_min, &mins[count]);
		PSVECSubtract(&hit_max, &hurt_max, &maxes[count]);
		VectorAverage(&hit_min_pos, &hurt_min_pos, &min_pos[count]);
		VectorAverage(&hit_max_pos, &hurt_max_pos, &max_pos[count]);
		texts[count++] = &disjoint_text[slot];
	}

	MinMaxPositions positions[3];
	GetMinMaxPositions(
		&player->position,
		mins,
		maxes,
		min_pos,
		max_pos,
		count,
		positions);

	for (int i = 0; i < count; i++) {
		MinMaxText_Update(
			texts[i],
			&positions[i],
			&mins[i],
			&maxes[i]);
	}
}

void HitboxStats_Update(void)
{
	if (NameTagText == NULL)
		return;

	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (!initialized[i])
			continue;

		// Hide any text that doesn't get updated this frame
		MinMaxText_Hide(&hitbox_text[i]);
		MinMaxText_Hide(&hurtbox_text[i]);
		MinMaxText_Hide(&disjoint_text[i]);
	}

	HSD_GObj *gobj = plinkhigh_gobjs[GOBJ_LINK_PLAYER];
	for (; gobj != NULL; gobj = gobj->next)
		HitboxStats_UpdatePlayer(gobj->data);
}

void HitboxStats_SceneInit(void)
{
	for (int i = 0; i < MAX_PLAYERS; i++)
		initialized[i] = FALSE;
}

void HitboxStats_Init(void)
{
	static MenuItem menu_description = {
		.text =
			"Displays numeric stats for hitboxes.\n"
			"Hitbox Extension:  Red\n"
			"Hurtbox Extension: Yellow\n"
			"Disjoint:          White\n",
		.type = MenuItem_Text
	};

	static MenuItem menu_hitbox_text_enabled = {
		.text = "Show Hitbox Extension",
		.type = MenuItem_AdjustEnum,
		.u = {
			.adjust_enum = {
				&hitbox_text_enabled,
				1, 0, 1, TRUE,
				OnOffText
			}
		}
	};

	static MenuItem menu_hurtbox_text_enabled = {
		.text = "Show Hurtbox Extension",
		.type = MenuItem_AdjustEnum,
		.u = {
			.adjust_enum = {
				&hurtbox_text_enabled,
				1, 0, 1, TRUE,
				OnOffText
			}
		}
	};

	static MenuItem menu_disjoint_text_enabled = {
		.text = "Show Disjoint",
		.type = MenuItem_AdjustEnum,
		.u = {
			.adjust_enum = {
				&disjoint_text_enabled,
				1, 0, 1, TRUE,
				OnOffText
			}
		}
	};

	Menu_Init(&hitbox_stats_menu, "Hitbox Stats");
	Menu_AddItem(&hitbox_stats_menu, &menu_description);
	Menu_AddItem(&hitbox_stats_menu, &menu_hitbox_text_enabled);
	Menu_AddItem(&hitbox_stats_menu, &menu_hurtbox_text_enabled);
	Menu_AddItem(&hitbox_stats_menu, &menu_disjoint_text_enabled);

	// Add to main menu
	static MenuItem menu_hitbox_stats = {
		.text = "Hitbox Stats",
		.type = MenuItem_SubMenu,
		.u = { .submenu = &hitbox_stats_menu }
	};

	Menu_AddItem(&MainMenu, &menu_hitbox_stats);
}