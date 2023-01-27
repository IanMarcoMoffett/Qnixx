/*
 *  Description: Cdefs header.
 *  Author(s): Ian Marco Moffett.
 */

#ifndef _INCLUDE_CDEFS_H_
#define _INCLUDE_CDEFS_H_


#define __dead        __attribute__((noreturn))
#define __aligned(n)  __attribute__((aligned(n)))
#define __unused      __attribute__((unused))
#define __packed      __attribute__((packed))
#define __isr         __attribute__((interrupt))
#define __asm         __asm__ __volatile__

#ifdef __cplusplus
#define __BEGIN_DECLS extern "C" {
#define __END_DECLS   }
#else
#define __BEGIN_DECLS /* Empty */
#define __END_DECLS   /* Empty */
#endif

#if defined(__STDC__) || defined(_AIX)              \
     || (defined(__mips) && defined(_SYSTYPE_SVR4)) \
     || defined(__cplusplus)
#define __P(protos) protos
#else
#define __P(protos) ()
#endif

#endif
