#include "util.h"
#include "hsd.h"
#include "melee.h"
#include <string.h>

// Start offset to copy the end of player struct
#define PLAYER_COPY_START 0x1830
#define PLAYER_COPY_SIZE (sizeof(Player) - PLAYER_COPY_START)

typedef struct _State_JObj {
    struct _State_JObj *next_sibling;
    struct _State_JObj *first_child;
    Quaternion rotation;
    Vector scale;
    Vector position;
} State_JObj;

typedef struct _State_Player {
    struct _State_JObj *jobj;

    // Action state, subaction, and animation
    u32 action_state;
    u32 subaction;
    SubactionState subaction_state;
    float animation_frame;
    float subaction_speed;
    float animation_speed;
    float anim_lerp_duration;
    float anim_lerp_progress;

    // Position, velocity, and scale
	float scale;
	Vector move_vel_delta;
	Vector move_vel;
	Vector knockback_vel;
	Vector pushback_vel;
	Vector move_vel_lerp;
	Vector position;
	Vector last_position;
	Vector delta_vel;
	u32 airborne;
	float ground_vel_delta;
	float ground_accel;
	float ground_vel;
	float ground_knockback_vel;
	float ground_pushback_vel;
	float jostle_delta_x;
	float jostle_delta_z;

    // Whole ass input struct
    PlayerInput input;

    // Whole ass physics struct
    Physics phys;

    // End of player struct
    char data[PLAYER_COPY_SIZE];
} State_Player;

typedef struct {
    HSD_GObj *gobj;
    void *data;
} State_GObj;

typedef struct _State_Frame {
    // Globals
    u32 RandomSeed;
    u32 StageAnimationFrame;

    // Objects
    int gobj_count;
    State_GObj *gobj_states[0];
} State_Frame;

State_Frame *saved_state = NULL;

/*
 * Recursively save state for all JObjs in the hierarchy.
 */
static void SaveState_JObj(HSD_JObj *jobj, State_JObj **out_state)
{
    if (jobj == NULL) {
        *out_state = NULL;
        return;
    }

    do {
        State_JObj *state = HSD_MemAlloc(sizeof(State_JObj));
        *out_state = state;

        state->rotation = jobj->rotation;
        state->scale = jobj->scale;
        state->position = jobj->position;

        SaveState_JObj(jobj->first_child, &state->first_child);

        jobj = jobj->next_sibling;
        out_state = &state->next_sibling;
    } while (jobj != NULL);
}

/*
 * Recursively load state for all JObjs in the hierarchy.
 */
static void LoadState_JObj(HSD_JObj *jobj, State_JObj *state)
{
    while (jobj != NULL && state != NULL) {
        jobj->rotation = state->rotation;
        jobj->scale = state->scale;
        jobj->position = state->position;
        jobj->flags |= MTX_DIRTY;

        LoadState_JObj(jobj->first_child, state->first_child);

        jobj = jobj->next_sibling;
        state = state->next_sibling;
    }
}

static void SaveState_Player(Player *player, State_Player **out_state)
{
    State_Player *state = HSD_MemAlloc(sizeof(State_Player));
    *out_state = state;

    // Save skeleton for lerping
    SaveState_JObj(player->gobj->hsd_obj, &state->jobj);

    state->action_state = player->action_state;
    state->subaction = player->subaction;
    state->subaction_state = player->subaction_state;
    state->animation_frame = player->animation_frame;
    state->subaction_speed = player->subaction_speed;
    state->animation_speed = player->animation_speed;
    state->anim_lerp_duration = player->anim_lerp_duration;
    state->anim_lerp_progress = player->anim_lerp_progress;

    state->scale = player->scale;
    state->move_vel_delta = player->move_vel_delta;
    state->move_vel = player->move_vel;
    state->knockback_vel = player->knockback_vel;
    state->pushback_vel = player->pushback_vel;
    state->move_vel_lerp = player->move_vel_lerp;
    state->position = player->position;
    state->last_position = player->last_position;
    state->delta_vel = player->delta_vel;
	state->airborne = player->airborne;
	state->ground_vel_delta = player->ground_vel_delta;
	state->ground_accel = player->ground_accel;
	state->ground_vel = player->ground_vel;
	state->ground_knockback_vel = player->ground_knockback_vel;
	state->ground_pushback_vel = player->ground_pushback_vel;
	state->jostle_delta_x = player->jostle_delta_x;
	state->jostle_delta_z = player->jostle_delta_z;

    state->input = player->input;
    state->phys = player->phys;

    memcpy(state->data, (char*)player + PLAYER_COPY_START, PLAYER_COPY_SIZE);
}

static void LoadState_Player(Player *player, State_Player *state)
{
    // Set the AS first because this overwrites flags and jobj data
    float lerp_left = state->anim_lerp_duration - state->anim_lerp_progress;
    if (lerp_left <= 0.f)
        lerp_left = -1.f;

    ActionStateChange(
        player->gobj,
        state->action_state,
        ASChangeFlag_SkipSubactionEvents,
        NULL,
        state->animation_frame,
        state->animation_speed,
        lerp_left);

    LoadState_JObj(player->gobj->hsd_obj, state->jobj);

    player->subaction = state->subaction;
    player->subaction_state = state->subaction_state;
    player->subaction_speed = state->subaction_speed;
    player->anim_lerp_duration = state->anim_lerp_duration;
    player->anim_lerp_progress = state->anim_lerp_progress;

    player->scale = state->scale;
    player->move_vel_delta = state->move_vel_delta;
    player->move_vel = state->move_vel;
    player->knockback_vel = state->knockback_vel;
    player->pushback_vel = state->pushback_vel;
    player->move_vel_lerp = state->move_vel_lerp;
    player->position = state->position;
    player->last_position = state->last_position;
    player->delta_vel = state->delta_vel;
	player->airborne = state->airborne;
	player->ground_vel_delta = state->ground_vel_delta;
	player->ground_accel = state->ground_accel;
	player->ground_vel = state->ground_vel;
	player->ground_knockback_vel = state->ground_knockback_vel;
	player->ground_pushback_vel = state->ground_pushback_vel;
	player->jostle_delta_x = state->jostle_delta_x;
	player->jostle_delta_z = state->jostle_delta_z;

    player->input = state->input;
    player->phys = state->phys;

    memcpy((char*)player + PLAYER_COPY_START, state->data, PLAYER_COPY_SIZE);
}

static void SaveState_GObj(HSD_GObj *gobj, State_GObj **out_state)
{
    State_GObj *state = HSD_MemAlloc(sizeof(State_GObj));
    *out_state = state;
    state->gobj = gobj;

    switch(gobj->p_link) {
    case GOBJ_LINK_PLAYER:
        SaveState_Player(gobj->data, (State_Player**)&state->data);
        break;
    }
}

static void LoadState_GObj(State_GObj *state)
{
    switch(state->gobj->p_link) {
    case GOBJ_LINK_PLAYER:
        LoadState_Player(state->gobj->data, state->data);
        break;
    }
}

static int CountObjects(int link)
{
	HSD_GObj *gobj = plinkhigh_gobjs[link];

    int result = 0;
	for (; gobj != NULL; gobj = gobj->next)
        result++;

    return result;
}

static void SaveObjects(State_Frame *state, int link)
{
	HSD_GObj *gobj = plinkhigh_gobjs[link];
	for (; gobj != NULL; gobj = gobj->next)
        SaveState_GObj(gobj, &state->gobj_states[state->gobj_count++]);
}

void SaveState(State_Frame **out_state)
{
    // Determine size with object state array
    int object_count = CountObjects(GOBJ_LINK_PLAYER);
    size_t state_size = sizeof(State_Frame) + sizeof(void*) * object_count;

    State_Frame *state = HSD_MemAlloc(state_size);
    *out_state = state;

    state->RandomSeed = RandomSeed;
    state->StageAnimationFrame = StageAnimationFrame;

    state->gobj_count = 0;
    SaveObjects(state, GOBJ_LINK_PLAYER);
}

void LoadState(State_Frame *state)
{
    RandomSeed = state->RandomSeed;
    StageAnimationFrame = state->StageAnimationFrame;

    for (int i = 0; i < state->gobj_count; i++)
        LoadState_GObj(state->gobj_states[i]);
}

void SaveStates_Update(void)
{
	for (int i = 0; i < 4; i++) {
		HSD_PadStatus *pad = &HSD_PadMasterStatus[i];
		if (pad->buttons == Button_DPadRight) {
            if (pad->instant_buttons == Button_DPadRight)
                SaveState(&saved_state);
        } else if (pad->buttons == Button_DPadLeft) {
            if (pad->instant_buttons == Button_DPadLeft)
                LoadState(saved_state);
        }
    }
}