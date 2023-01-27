#ifndef _FS_VFS_INIT_H_
#define _FS_VFS_INIT_H_

#include <types.h>
#include <sys/vnode.h>

/*
 *  Sets up the VFS.
 */

void vfs_init(void);

/*
 *  Creates a VFS node.
 *
 *  @param name: Filename.
 *  @param vtype: Vnode type.
 *  @param is_mountpoint: 1 if mountpoint.
 */

vnode_t* vfs_make_vnode(const char* name, vtype_t vtype, 
                        uint8_t is_mountpoint);

/*
 *  Appends a child to a parent.
 *
 *  Set parent to NULL for the
 *  filesystem root.
 */

void vfs_append_child(vnode_t* parent, vnode_t* child);

#endif
