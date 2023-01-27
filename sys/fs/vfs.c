#include <fs/vfs.h>
#include <mm/heap.h>
#include <limits.h>
#include <string.h>
#include <lib/assert.h>
#include <tty/console.h>
#include <cdefs.h>          /* TODO: Remove this */

static vnode_t* fs_root = NULL;

static inline vnode_t*
name2vnode(const char* name, vnode_t* parent)
{
  return hashmap_get(&parent->children, name, strlen(name));
}

/*
 *  Looks up a vnode from a path.
 */

__unused static vnode_t*
path2vnode(const char* path)
{
  /* Skip '/' */
  if (*path != '/')
  {
    return NULL;
  }
  ++path;
  
  /* 
   * @current: Current vnode
   * @ptr: Ptr to current character
   */
  vnode_t* current = fs_root;
  const char* ptr = path;
  
  /*
   *  @fname: Current filename.
   *  @fname_idx: Current filename's index.
   */
  char fname[NAME_MAX + 1];
  size_t fname_idx = 0;

  /* Start parsing */
  while (1)
  {
    if (fname_idx >= NAME_MAX)
    {
      return NULL;
    }

    if (*ptr == '/' || *ptr == '\0')
    {
      /* Got one filename */
      fname[fname_idx] = '\0';
      current = name2vnode(fname, current); 

      printk("%s\n", fname);
      
      if (current == NULL)
      {
        /* Not found */
        return NULL;
      }
      
      memzero(fname, sizeof(fname));
      fname_idx = 0;
      ++ptr;
    
      if (*(ptr - 1) == '\0')
      { 
        return current != fs_root ? current : NULL;
      }
    
      continue;
    }

    fname[fname_idx++] = *ptr++;
  }
  
  __builtin_unreachable();
}


void
vfs_append_child(vnode_t* parent, vnode_t* child)
{
  if (parent == NULL)
  {
    parent = fs_root;
  }

  hashmap_put(&parent->children, child->v_name, strlen(child->v_name),
              child);
}

vnode_t*
vfs_make_vnode(const char* name, vtype_t vtype, 
               uint8_t is_mountpoint)
{
  if (strlen(name) >= NAME_MAX)
  {
    /* Name too long! */
    return NULL;
  }

  vnode_t* new = kcalloc(1, sizeof(vnode_t));
  new->v_type = vtype;
  new->v_state = VSTATE_CONSTRUCTED;
  new->v_flags = is_mountpoint ? V_MOUNTPOINT : 0;
  new->n_children = 0;
  assert(hashmap_create(2, &new->children) == 0);
  memcpy(new->v_name, name, strlen(name));
  return new;
}

void vfs_init(void)
{
  fs_root = vfs_make_vnode("", VDIR, 1);
  fs_root->v_flags |= V_ROOT;
  printk(KERN_INFO "Mounted filesystem root at '/'\n");
}
