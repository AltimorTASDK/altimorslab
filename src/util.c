#include "util.h"
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