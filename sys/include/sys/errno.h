#ifndef _SYS_ERRNO_H_
#define _SYS_ERRNO_H_

/*
 *  Description: Error numbers
 *  Author(s): Ian Marco Moffett
 *
 */


#define E2BIG             2     /* Argument list too long */
#define EACCES            3     /* Permission denied */
#define EADDRINUSE        4     /* Address in use */
#define EADDRNOTAVAIL     5     /* Address not available */
#define EAFNOSUPPORT      6     /* Address family not supported */
#define EAGAIN            7     /* Resource unavailable, try again */
#define EALREADY          8     /* Connection already in progress */
#define EBADF             9     /* Bad file descriptor */
#define EBUSY             10    /* Device or resource busy */
#define ECANCELED         11    /* Operation canceled */
#define ECHILD            12    /* No child processes */
#define ECONNABORTED      13    /* Connection aborted */
#define ECONNRESET        14    /* Connection reset */
#define ECONNREFUSED      15    /* Connection refused */
#define EDEADLK           16    /* Resource deadlock would occur */
#define EDESTADDRREQ      17    /* Destination address required */
#define EDOM              18    /* Mathematics argument out of domain of function */
#define EEXIST            19    /* File exists */
#define EFAULT            20    /* Bad address */
#define EFBIG             21    /* File too large */
#define EHOSTUNREACH      22    /* Host is unreachable */
#define EIDRM             23    /* Identifier removed */
#define EILSEQ            24    /* Illegal byte sequence */
#define EINPROGRESS       25    /* Operation in progress */
#define EINTR             26    /* Interrupted function */
#define EINVAL            27    /* Invalid argument */
#define EIO               28    /* I/O error */
#define EISDIR            29    /* Is a directory */
#define ELOOP             30    /* Too many levels of symbolic links */
#define EMFILE            31    /* File descriptor value too large */
#define EMLINK            32    /* Too many links */
#define ENAMETOOLONG      33    /* Filename too long */
#define ENETDOWN          34    /* Network is down */
#define ENOENT            35    /* No such file or directory */

#endif
