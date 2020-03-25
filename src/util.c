#include "util.h"
#include <stdlib.h>

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