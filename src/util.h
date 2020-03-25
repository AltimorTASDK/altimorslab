#pragma once

#define max(a, b) (a > b ? a : b)
#define min(a, b) (a < b ? a : b)

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