/*
 * Define hooks as pairs of words containing the original function address
 * and the hook address. patch_dol.py applies these and replaces the hook
 * descriptor with the overwritten instruction from the patch and a branch back
 * to the original.
 */
.section .hooks

.macro add_hook name
	.global orig_\name
	orig_\name:
	.int \name
	.int hook_\name
.endm

add_hook main
add_hook ActionStateChange
add_hook Stage_CheckStageUnlockStatus
add_hook Characters_CheckCharUnlockStatus
add_hook SinglePlayerModeCheck
add_hook StartMelee
add_hook DevelopMode_IngameTogglesMost
add_hook wP_RunObjectFrameFunctions
add_hook DevelopText_DrawAll
add_hook ColorData_HandleStuff
add_hook HSD_JObjAnim
add_hook Player_MeleeDamage
add_hook Player_UpdateActionInputTimers
add_hook Physics_Move
