/*
 *  Description: Queue implementation.
 *  Author(s): Ian Marco Moffett.
 *
 */

#ifndef _SYS_QUEUE_H_
#define _SYS_QUEUE_H_

#include <types.h>

/*
 *  Head of queue.
 */

#define TAILQ_HEAD(name, type)              \
  struct name                               \
  {                                         \
    struct type* first;                     \
    struct type** last;                     \
  }

/*
 *  Holds pointers to next and previous
 *  entries.
 */

#define TAILQ_ENTRY(type)                   \
  struct                                    \
  {                                         \
    struct type* next;                      \
    struct type** prev;                     \
  }

/*
 *  Tailq access methods.
 */

#define TAILQ_FIRST(head) ((head)->first)
#define TAILQ_NEXT(elm, field) ((elm)->field.next)
#define TAILQ_LAST(head, headname) \
    (*(((struct headname*)((head)->last))->last))

#define TAILQ_PREV(elm, headname, field) \
  (*(((struct headname *)((elm)->field.prev))->last))

#define TAILQ_EMPTY(head) \
  (TAILQ_FIRST(head) == NULL)

#define TAILQ_FOREACH_SAFE(var, head, field, tvar)      \
  for ((var) = TAILQ_FIRST(head);                       \
      (var) != NULL &&                                  \
      ((tvar) = TAILQ_NEXT(var, field), 1);             \
      (var) = (tvar))

#define TAILQ_FOREACH_REVERSE(var, head, headname, field)     \
  for((var) = TAILQ_LAST(head, headname);                     \
      (var) != NULL;                                          \
      (var) = TAILQ_PREV(var, headname, field))

/*
 *  Tail queue functions.
 */

#define TAILQ_INIT(head)                \
  do                                    \
  {                                     \
    (head)->first = NULL;               \
    (head)->last = &(head)->first;      \
  } while (0);

#define TAILQ_INSERT_HEAD(head, elm, field) do {          \
  if (((elm)->field.next = (head)->first) != NULL)        \
    (head)->first->field.prev =                           \
        &(elm)->field.next;                               \
  else                                                    \
    (head)->last = &(elm)->field.next;                    \
  (head)->first = (elm);                                  \
  (elm)->field.prev = &(head)->first;                     \
} while (0)

#define TAILQ_INSERT_TAIL(head, elm, field) do {    \
  (elm)->field.next = NULL;                         \
  (elm)->field.prev = (head)->last;                 \
  *(head)->last = (elm);                            \
  (head)->last = &(elm)->field.next;                \
} while (0)

#endif
