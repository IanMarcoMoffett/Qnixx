/*
 *  Description: Stat header.
 *  Author(s): Ian Marco Moffett.
 */

#ifndef _SYS_STAT_H_
#define _SYS_STAT_H_

#include <sys/types.h>

#define S_IFBLK 0   /* Block special */
#define S_IFCHR 1   /* Character special */
#define S_IFIFO 2   /* FIFO special */
#define S_IFREG 3   /* Regular */
#define S_IFDIR 4   /* Directory */
#define S_IFLNK 5   /* Symbolic link */

#endif
