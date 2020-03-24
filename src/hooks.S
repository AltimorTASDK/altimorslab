/*
 * Define hooks as pairs of words containing the original function address
 * and the hook address. patch_dol.py applies these and replaces the hook
 * descriptor with the overwritten instruction from the patch and a branch back
 * to the original.
 */
.section .hooks

.global orig_ActionStateChange
orig_ActionStateChange:
.int ActionStateChange
.int hook_ActionStateChange

.int Stage_CheckStageUnlockStatus
.int hook_Stage_CheckStageUnlockStatus

.int Characters_CheckCharUnlockStatus
.int hook_Characters_CheckCharUnlockStatus

.int SinglePlayerModeCheck
.int hook_SinglePlayerModeCheck

.global orig_StartMelee
orig_StartMelee:
.int StartMelee
.int hook_StartMelee

.global orig_DevelopMode_IngameTogglesMost
orig_DevelopMode_IngameTogglesMost:
.int DevelopMode_IngameTogglesMost
.int hook_DevelopMode_IngameTogglesMost
