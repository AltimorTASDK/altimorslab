#include "hsd.h"
#include "melee.h"
#include <string.h>

const char *action_state_names[AS_NAMED_MAX] = {
	"DeadDown",
	"DeadLeft",
	"DeadRight",
	"DeadUp",
	"DeadUpStar",
	"DeadUpStarIce",
	"DeadUpFall",
	"DeadUpFallHitCamera",
	"DeadUpFallHitCameraFlat",
	"DeadUpFallIce",
	"DeadUpFallHitCameraIce",
	"Sleep",
	"Rebirth",
	"RebirthWait",
	"Wait",
	"WalkSlow",
	"WalkMiddle",
	"WalkFast",
	"Turn",
	"TurnRun",
	"Dash",
	"Run",
	"RunDirect",
	"RunBrake",
	"KneeBend",
	"JumpF",
	"JumpB",
	"JumpAerialF",
	"JumpAerialB",
	"Fall",
	"FallF",
	"FallB",
	"FallAerial",
	"FallAerialF",
	"FallAerialB",
	"FallSpecial",
	"FallSpecialF",
	"FallSpecialB",
	"DamageFall",
	"Squat",
	"SquatWait",
	"SquatRv",
	"Landing",
	"LandingFallSpecial",
	"Attack11",
	"Attack12",
	"Attack13",
	"Attack100Start",
	"Attack100Loop",
	"Attack100End",
	"AttackDash",
	"AttackS3Hi",
	"AttackS3HiS",
	"AttackS3S",
	"AttackS3LwS",
	"AttackS3Lw",
	"AttackHi3",
	"AttackLw3",
	"AttackS4Hi",
	"AttackS4HiS",
	"AttackS4S",
	"AttackS4LwS",
	"AttackS4Lw",
	"AttackHi4",
	"AttackLw4",
	"AttackAirN",
	"AttackAirF",
	"AttackAirB",
	"AttackAirHi",
	"AttackAirLw",
	"LandingAirN",
	"LandingAirF",
	"LandingAirB",
	"LandingAirHi",
	"LandingAirLw",
	"DamageHi1",
	"DamageHi2",
	"DamageHi3",
	"DamageN1",
	"DamageN2",
	"DamageN3",
	"DamageLw1",
	"DamageLw2",
	"DamageLw3",
	"DamageAir1",
	"DamageAir2",
	"DamageAir3",
	"DamageFlyHi",
	"DamageFlyN",
	"DamageFlyLw",
	"DamageFlyTop",
	"DamageFlyRoll",
	"LightGet",
	"HeavyGet",
	"LightThrowF",
	"LightThrowB",
	"LightThrowHi",
	"LightThrowLw",
	"LightThrowDash",
	"LightThrowDrop",
	"LightThrowAirF",
	"LightThrowAirB",
	"LightThrowAirHi",
	"LightThrowAirLw",
	"HeavyThrowF",
	"HeavyThrowB",
	"HeavyThrowHi",
	"HeavyThrowLw",
	"LightThrowF4",
	"LightThrowB4",
	"LightThrowHi4",
	"LightThrowLw4",
	"LightThrowAirF4",
	"LightThrowAirB4",
	"LightThrowAirHi4",
	"LightThrowAirLw4",
	"HeavyThrowF4",
	"HeavyThrowB4",
	"HeavyThrowHi4",
	"HeavyThrowLw4",
	"SwordSwing1",
	"SwordSwing3",
	"SwordSwing4",
	"SwordSwingDash",
	"BatSwing1",
	"BatSwing3",
	"BatSwing4",
	"BatSwingDash",
	"ParasolSwing1",
	"ParasolSwing3",
	"ParasolSwing4",
	"ParasolSwingDash",
	"HarisenSwing1",
	"HarisenSwing3",
	"HarisenSwing4",
	"HarisenSwingDash",
	"StarRodSwing1",
	"StarRodSwing3",
	"StarRodSwing4",
	"StarRodSwingDash",
	"LipStickSwing1",
	"LipStickSwing3",
	"LipStickSwing4",
	"LipStickSwingDash",
	"ItemParasolOpen",
	"ItemParasolFall",
	"ItemParasolFallSpecial",
	"ItemParasolDamageFall",
	"LGunShoot",
	"LGunShootAir",
	"LGunShootEmpty",
	"LGunShootAirEmpty",
	"FireFlowerShoot",
	"FireFlowerShootAir",
	"ItemScrew",
	"ItemScrewAir",
	"DamageScrew",
	"DamageScrewAir",
	"ItemScopeStart",
	"ItemScopeRapid",
	"ItemScopeFire",
	"ItemScopeEnd",
	"ItemScopeAirStart",
	"ItemScopeAirRapid",
	"ItemScopeAirFire",
	"ItemScopeAirEnd",
	"ItemScopeStartEmpty",
	"ItemScopeRapidEmpty",
	"ItemScopeFireEmpty",
	"ItemScopeEndEmpty",
	"ItemScopeAirStartEmpty",
	"ItemScopeAirRapidEmpty",
	"ItemScopeAirFireEmpty",
	"ItemScopeAirEndEmpty",
	"LiftWait",
	"LiftWalk1",
	"LiftWalk2",
	"LiftTurn",
	"GuardOn",
	"Guard",
	"GuardOff",
	"GuardSetOff",
	"GuardReflect",
	"DownBoundU",
	"DownWaitU",
	"DownDamageU",
	"DownStandU",
	"DownAttackU",
	"DownFowardU",
	"DownBackU",
	"DownSpotU",
	"DownBoundD",
	"DownWaitD",
	"DownDamageD",
	"DownStandD",
	"DownAttackD",
	"DownFowardD",
	"DownBackD",
	"DownSpotD",
	"Passive",
	"PassiveStandF",
	"PassiveStandB",
	"PassiveWall",
	"PassiveWallJump",
	"PassiveCeil",
	"ShieldBreakFly",
	"ShieldBreakFall",
	"ShieldBreakDownU",
	"ShieldBreakDownD",
	"ShieldBreakStandU",
	"ShieldBreakStandD",
	"FuraFura",
	"Catch",
	"CatchPull",
	"CatchDash",
	"CatchDashPull",
	"CatchWait",
	"CatchAttack",
	"CatchCut",
	"ThrowF",
	"ThrowB",
	"ThrowHi",
	"ThrowLw",
	"CapturePulledHi",
	"CaptureWaitHi",
	"CaptureDamageHi",
	"CapturePulledLw",
	"CaptureWaitLw",
	"CaptureDamageLw",
	"CaptureCut",
	"CaptureJump",
	"CaptureNeck",
	"CaptureFoot",
	"EscapeF",
	"EscapeB",
	"Escape",
	"EscapeAir",
	"ReboundStop",
	"Rebound",
	"ThrownF",
	"ThrownB",
	"ThrownHi",
	"ThrownLw",
	"ThrownLwWomen",
	"Pass",
	"Ottotto",
	"OttottoWait",
	"FlyReflectWall",
	"FlyReflectCeil",
	"StopWall",
	"StopCeil",
	"MissFoot",
	"CliffCatch",
	"CliffWait",
	"CliffClimbSlow",
	"CliffClimbQuick",
	"CliffAttackSlow",
	"liffAttackQuick",
	"CliffEscapeSlow",
	"CliffEscapeQuick",
	"CliffJumpSlow1",
	"CliffJumpSlow2",
	"CliffJumpQuick1",
	"CliffJumpQuick2",
	"AppealR",
	"AppealL",
	"ShoulderedWait",
	"ShoulderedWalkSlow",
	"ShoulderedWalkMiddle",
	"ShoulderedWalkFast",
	"ShoulderedTurn",
	"ThrownFF",
	"ThrownFB",
	"ThrownFHi",
	"ThrownFLw",
	"CaptureCaptain",
	"CaptureYoshi",
	"YoshiEgg",
	"CaptureKoopa",
	"CaptureDamageKoopa",
	"CaptureWaitKoopa",
	"ThrownKoopaF",
	"ThrownKoopaB",
	"CaptureKoopaAir",
	"CaptureDamageKoopaAir",
	"CaptureWaitKoopaAir",
	"ThrownKoopaAirF",
	"ThrownKoopaAirB",
	"CaptureKirby",
	"CaptureWaitKirby",
	"ThrownKirbyStar",
	"ThrownCopyStar",
	"ThrownKirby",
	"BarrelWait",
	"Bury",
	"BuryWait",
	"BuryJump",
	"DamageSong",
	"DamageSongWait",
	"DamageSongRv",
	"DamageBind",
	"CaptureMewtwo",
	"CaptureMewtwoAir",
	"ThrownMewtwo",
	"ThrownMewtwoAir",
	"WarpStarJump",
	"WarpStarFall",
	"HammerWait",
	"HammerWalk",
	"HammerTurn",
	"HammerKneeBend",
	"HammerFall",
	"HammerJump",
	"HammerLanding",
	"KinokoGiantStart",
	"KinokoGiantStartAir",
	"KinokoGiantEnd",
	"KinokoGiantEndAir",
	"KinokoSmallStart",
	"KinokoSmallStartAir",
	"KinokoSmallEnd",
	"KinokoSmallEndAir",
	"Entry",
	"EntryStart",
	"EntryEnd",
	"DamageIce",
	"DamageIceJump",
	"CaptureMasterhand",
	"CapturedamageMasterhand",
	"CapturewaitMasterhand",
	"ThrownMasterhand",
	"CaptureKirbyYoshi",
	"KirbyYoshiEgg",
	"CaptureLeadead",
	"CaptureLikelike",
	"DownReflect",
	"CaptureCrazyhand",
	"CapturedamageCrazyhand",
	"CapturewaitCrazyhand",
	"ThrownCrazyhand",
	"BarrelCannonWait"
};

/*
 * Print a dev text message using \x01 through \x04 as color codes
 */
void DevelopText_ColorPrint(DevText *text, const char *message)
{
	const char *start = message;
	while (*message != 0) {
		if (*message < 1 || *message > 4) {
			message++;
			continue;
		}

		// Print message up until color code
		size_t length = (size_t)(message - start);
		if (length > 0) {
			char *buf = HSD_MemAlloc(length + 1);
			memcpy(buf, start, length);
			buf[length] = '\0';
			DevelopText_Print(text, buf);
		}

		DevelopText_StoreColorIndex(text, *message - 1);
		start = ++message;
	}

	DevelopText_Print(text, start);
	DevelopText_StoreColorIndex(text, 0);
}