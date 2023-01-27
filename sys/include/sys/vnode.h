/*
 *  Description: VFS node header.
 *  Author(s): Ian Marco Moffett.
 */

#ifndef _SYS_VNODE_H_
#define _SYS_VNODE_H_

#ifdef _KERNEL
#include <sync/mutex.h>
#include <mm/heap.h>
#include <lib/hashmap.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <cdefs.h>


__BEGIN_DECLS

struct Vnode;
typedef uint8_t vflags_t;

/* Vnode types */
typedef enum
{
  VNON,           /* No type */
  VREG,           /* Regular */
  VDIR,           /* Directory */
  VBLK,           /* Block device */
  VCHR,           /* Char device */
  VBAD,           /* Bad */
} vtype_t;

#ifdef _KERNEL

/* Vnode states */
typedef enum
{
  VSTATE_UNINIT,        /* Uninitialized */
  VSTATE_CONSTRUCTED,
  VSTATE_DESTROYING,
  VSTATE_DEAD
} vstate_t;

/*
 *  Vnode operations.
 */

typedef struct
{
  int(*mkdir)(struct Vnode* vnode, const char* name, mode_t mode);
} vnode_ops_t;

/*
 *  Actual vnode
 */

typedef struct Vnode
{
  char        v_name[255];     /* Vnode name */
  vtype_t     v_type;          /* Vnode type */
  vstate_t    v_state;         /* Vnode state */
  vflags_t    v_flags;         /* Vnode flags */
  mutex_t     v_lock;          /* Vnode lock */
  vnode_ops_t v_ops;           /* Vnode fops */ 

  /* Children */
  struct hashmap_s children;
  size_t n_children;
} vnode_t;


/* 
 *  Helper macros.
 */

#define V_LOCK(vnode) mutex_acquire(&(vnode)->v_lock)
#define V_UNLOCK(vnode) mutex_release(&(vnode)->v_lock)

#endif      /* _KERNEL */

/*
 *  Vnode flags.
 *
 */

#define V_ROOT        (1 << 0)      /* Root of filesystem */
#define V_ETERNALDEV  (1 << 1)      /* Device that is never destroyed */
#define V_SYSTEM      (1 << 2)      /* Vnode is being used by kernel */
#define V_MOUNTPOINT  (1 << 3)      /* Vnode is a mountpoint */

__END_DECLS

#endif    /* _SYS_VNODE_H_ */
