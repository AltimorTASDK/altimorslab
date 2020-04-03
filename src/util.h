#pragma once

#include "hsd.h"
#include <math.h>

// Useful for calculating log base 10 with logf
#define LOG10 2.30258509299f

#define max(a, b) (a > b ? a : b)
#define min(a, b) (a < b ? a : b)

#define deg2rad(x) (x * M_PI / 180.f)
#define rad2deg(x) (x * 180.f / M_PI)

typedef enum _Direction {
	Dir_None,
	Dir_Right,
	Dir_Up,
	Dir_Left,
	Dir_Down
} Direction;

typedef char BOOL;
#define TRUE ((BOOL)1)
#define FALSE ((BOOL)0)

/*
 * Circular linked list where the head is a pseudo-element pointed to by the
 * first and last elements
 */
typedef struct _ListLink {
	struct _ListLink *next;
	struct _ListLink *prev;
} ListLink;

void List_Init(ListLink *head);
void List_Append(ListLink *head, ListLink *elem);
void List_Prepend(ListLink *head, ListLink *elem);
void List_Remove(ListLink *elem);

// Math
void EulerToQuaternion(float pitch, float yaw, float roll, Quaternion *out);

static inline float Lerp(float a, float b, float c)
{
	if (c <= 0.f)
		return a;
	if (c >= 1.f)
		return b;

	return a + (b - a) * c;
}

static inline float InvLerp(float value, float a, float b)
{
	if (value <= a)
		return 0.f;
	if (value >= b)
		return 1.f;

	return (value - a) / (b - a);
}

static inline void VectorMin(Vector *a, Vector *b, Vector *out)
{
	out->x = min(a->x, b->x);
	out->y = min(a->y, b->y);
	out->z = min(a->z, b->z);
}

static inline void VectorMax(Vector *a, Vector *b, Vector *out)
{
	out->x = max(a->x, b->x);
	out->y = max(a->y, b->y);
	out->z = max(a->z, b->z);
}

static inline void VectorAverage(Vector *a, Vector *b, Vector *out)
{
	PSVECAdd(a, b, out);
	PSVECScale(out, .5f, out);
}

static inline float VectorDistance(Vector *a, Vector *b)
{
	Vector difference;
	PSVECSubtract(a, b, &difference);
	return PSVECMag(&difference);
}