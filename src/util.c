#include "util.h"
#include "gx.h"
#include <stdlib.h>
#include <math.h>

void List_Init(ListLink *head)
{
	head->next = head->prev = head;
}

void List_Append(ListLink *head, ListLink *elem)
{
	elem->next = head;
	elem->prev = head->prev;
	head->prev->next = elem;
	head->prev = elem;
}

void List_Prepend(ListLink *head, ListLink *elem)
{
	elem->prev = head;
	elem->next = head->next;
	head->next->prev = elem;
	head->next = elem;
}

void List_Remove(ListLink *elem)
{
	elem->prev->next = elem->next;
	elem->next->prev = elem->prev;
}

void EulerToQuaternion(float pitch, float yaw, float roll, Quaternion *out)
{
	float cp = cosf(pitch / 2);
	float sp = sinf(pitch / 2);
	float cy = cosf(yaw / 2);
	float sy = sinf(yaw / 2);
	float cr = cosf(roll / 2);
	float sr = sinf(roll / 2);
	out->x = sr * cp * cy - cr * sp * sy;
	out->y = cr * sp * cy + sr * cp * sy;
	out->z = cr * cp * sy - sr * sp * cy;
	out->w = cr * cp * cy + sr * sp * sy;
}

void WriteVertex2D(float x, float y, u32 color)
{
	GX_WRITE_FLOAT(x);
	GX_WRITE_FLOAT(y);
	GX_WRITE_FLOAT(0.f);
	GX_WRITE_U8(get_color_r(color));
	GX_WRITE_U8(get_color_g(color));
	GX_WRITE_U8(get_color_b(color));
	GX_WRITE_U8(get_color_a(color));
}

void FillRectangle2D(float x, float y, float w, float h, u32 color)
{
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	WriteVertex2D(x, y, color);
	WriteVertex2D(x + w, y, color);
	WriteVertex2D(x + w, y + h, color);
	WriteVertex2D(x, y + h, color);
}

void FillCircle2D(float x, float y, float radius, int segments, u32 color)
{
	GXBegin(GX_TRIANGLEFAN, GX_VTXFMT0, segments + 2);
	WriteVertex2D(x, y, color);

	float angle_stride = 2 * M_PI / segments;
	for (int i = 0; i < segments + 1; i++) {
		float angle = i * angle_stride;
		WriteVertex2D(
			x + radius * cosf(angle),
			y + radius * sinf(angle),
			color);
	}
}

void StrokeCircle2D(float x, float y, float radius, int segments, u32 color)
{
	GXBegin(GX_LINESTRIP, GX_VTXFMT0, segments + 1);

	float angle_stride = 2 * M_PI / segments;
	for (int i = 0; i < segments + 1; i++) {
		float angle = i * angle_stride;
		WriteVertex2D(
			x + radius * cosf(angle),
			y + radius * sinf(angle),
			color);
	}
}