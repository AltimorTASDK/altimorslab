#include "util.h"
#include "hsd.h"
#include "melee.h"

typedef struct _State_JObj {
    struct _State_JObj *next_sibling;
    struct _State_JObj *first_child;
    Quaternion rotation;
    Vector scale;
    Vector position;
} State_JObj;

typedef struct _State_Player {
    struct _State_JObj *jobj;
} State_Player;

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

    SaveState_JObj(player->gobj->hsd_obj, &state->jobj);
}

void SaveState(void)
{

}