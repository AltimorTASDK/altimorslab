#pragma once

#include "int_types.h"
#include <stddef.h>

#define GOBJ_LINK_PLAYER 8

struct _HSD_GObj;
struct _HSD_JObj;
struct _HSD_CObj;

typedef struct _HSD_JObj HSD_JObj;
typedef struct _HSD_CObj HSD_CObj;

typedef struct _Vector2D {
	float x, y;
} Vector2D;

typedef struct _Vector {
	float x, y, z;
} Vector;

typedef struct _Quaternion {
	float x, y, z, w;
} Quaternion;

typedef struct _Matrix3x4 {
	float m[12];
} Matrix3x4;

typedef struct _Matrix4x4 {
	float m[16];
} Matrix4x4;

// Taken from FRAY, thanks to psiLupan
typedef struct _HSD_Class {
	struct _HSD_ClassInfo *class_info;
} HSD_Class;

typedef struct _HSD_Obj {
	HSD_Class parent;
	s16 ref_count;
	s16 ref_count_individual;
} HSD_Obj;

typedef struct _HSD_JObj {
	HSD_Obj base;
	HSD_JObj *next; // Next bone with the same parent
	HSD_JObj *parent;
	HSD_JObj *child; // First child
	u32 flags;
	union {
		struct _HSD_SList *ptcl;
		struct _HSD_DObj *dobj;
		struct _HSD_Spline *spline;
	} u;
	Quaternion rotation;
	char pad0028[0x2C - 0x28];
	Vector scale;
	Vector position;
	Matrix3x4 mtx;
	Vector *pvec;
	Matrix3x4 *vmtx;
	struct _HSD_AObj *aobj;
	struct _HSD_RObj *robj;
	struct _HSD_JObjDesc *desc;
} HSD_JObj;

typedef struct _HSD_GObjProc {
	struct _HSD_GObjProc *child;
	struct _HSD_GObjProc *next;
	struct _HSD_GObjProc *prev;
	u8 s_link;
	s8 flags;
	char align000E[2];
	struct _HSD_GObj *gobj;
	void (*callback)(struct _HSD_GObj*);
} HSD_GObjProc;

typedef struct _HSD_GObj {
	u16 classifier;
	s8 p_link;
	s8 gx_link;
	u8 p_priority;
	u8 render_priority;
	s8 obj_kind;
	s8 user_data_kind;
	struct _HSD_GObj *next;
	struct _HSD_GObj *prev;
	struct _HSD_GObj *next_gx;
	struct _HSD_GObj *prev_gx;
	HSD_GObjProc *proc;
	void (*render_cb)(struct _HSD_GObj *gobj, int code);
	u64 gxlink_prios;
	void *hsd_gobj;
	void *data;
	void (*user_data_remove_func)(void *data);
	void *pad0034;
} HSD_GObj;

extern HSD_GObj **plinkhigh_gobjs;

void *HSD_MemAlloc(size_t size);

// Math
void PSMTXMultVec(Matrix3x4 *mtx, Vector *vec, Vector *out);
void PSVECAdd(Vector *a, Vector *b, Vector *out);
void PSVECSubtract(Vector *a, Vector *b, Vector *out);
float PSVECMag(Vector *vec);
void PSVECScale(Vector *in, float scale, Vector *out);
void PSVECNormalize(Vector *in, Vector *out);

// GObj
HSD_GObjProc *GObj_CreateProcWithCallback(
	HSD_GObj *gobj,
	void (*cb)(),
	u8 s_prio);

// CObj
void makeProjectionMtx(HSD_CObj *cobj, Matrix4x4 *out);
void HSD_CObjTransform(
	HSD_CObj *cobj,
	Vector *pos3d,
	Vector *pos2d,
	int no_mtx_update);