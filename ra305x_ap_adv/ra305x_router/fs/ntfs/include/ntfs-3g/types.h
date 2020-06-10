/*
 * types.h - Misc type definitions not related to on-disk structure.  
 *           Originated from the Linux-NTFS project.
 *
 * Copyright (c) 2000-2004 Anton Altaparmakov
 *
 * This program/include file is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program/include file is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (in the main directory of the NTFS-3G
 * distribution in the file COPYING); if not, write to the Free Software
 * Foundation,Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _NTFS_TYPES_H
#define _NTFS_TYPES_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if HAVE_STDINT_H || !HAVE_CONFIG_H
//#include <stdint.h>
#endif
//#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
//#endif

typedef unsigned char u8;			/* Unsigned types of an exact size */
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef __signed char   s8;			/* Signed types of an exact size */
typedef short s16;
typedef int s32;
typedef long long s64;
//typedef long long size_t;


//typedef long off_t;


typedef u16 le16;
typedef u32 le32;
typedef u64 le64;

/*
 * Declare sle{16,32,64} to be unsigned because we do not want sign extension
 * on BE architectures.
 */
typedef u16 sle16;
typedef u32 sle32;
typedef u64 sle64;

typedef u16 ntfschar;			/* 2-byte Unicode character type. */
#define UCHAR_T_SIZE_BITS 1

/*
 * Clusters are signed 64-bit values on NTFS volumes.  We define two types, LCN
 * and VCN, to allow for type checking and better code readability.
 */
typedef s64 VCN;
typedef sle64 leVCN;
typedef s64 LCN;
typedef sle64 leLCN;

/*
 * The NTFS journal $LogFile uses log sequence numbers which are signed 64-bit
 * values.  We define our own type LSN, to allow for type checking and better
 * code readability.
 */
typedef s64 LSN;
typedef sle64 leLSN;

/*
 * Cygwin has a collision between our BOOL and <windef.h>'s
 * As long as this file will be included after <windows.h> were fine.
 */
#ifndef _WINDEF_H
/**
 * enum BOOL - These are just to make the code more readable...
 */
typedef enum {
#ifndef FALSE
	FALSE = 0,
#endif
#ifndef NO
	NO = 0,
#endif
#ifndef ZERO
	ZERO = 0,
#endif
#ifndef TRUE
	TRUE = 1,
#endif
#ifndef YES
	YES = 1,
#endif
#ifndef ONE
	ONE = 1,
#endif
} BOOL;
#endif /* defined _WINDEF_H */

/**
 * enum IGNORE_CASE_BOOL -
 */
typedef enum {
	CASE_SENSITIVE = 0,
	IGNORE_CASE = 1,
} IGNORE_CASE_BOOL;

#define STATUS_OK				(0)
#define STATUS_ERROR				(-1)
#define STATUS_RESIDENT_ATTRIBUTE_FILLED_MFT	(-2)
#define STATUS_KEEP_SEARCHING			(-3)
#define STATUS_NOT_FOUND			(-4)

#define	ENOMSG		35	/* No message of desired type */
#define	EIDRM		36	/* Identifier removed */
#define	ECHRNG		37	/* Channel number out of range */
#define	EL2NSYNC	38	/* Level 2 not synchronized */
#define	EL3HLT		39	/* Level 3 halted */
#define	EL3RST		40	/* Level 3 reset */
#define	ELNRNG		41	/* Link number out of range */
#define	EUNATCH		42	/* Protocol driver not attached */
#define	ENOCSI		43	/* No CSI structure available */
#define	EL2HLT		44	/* Level 2 halted */
#define	EDEADLK		45	/* Resource deadlock would occur */
#define	ENOLCK		46	/* No record locks available */
#define	EBADE		50	/* Invalid exchange */
#define	EBADR		51	/* Invalid request descriptor */
#define	EXFULL		52	/* Exchange full */
#define	ENOANO		53	/* No anode */
#define	EBADRQC		54	/* Invalid request code */
#define	EBADSLT		55	/* Invalid slot */
#define	EDEADLOCK	56	/* File locking deadlock error */
#define	EBFONT		59	/* Bad font file format */
#define	ENOSTR		60	/* Device not a stream */
#define	ENODATA		61	/* No data available */
#define	ETIME		62	/* Timer expired */
#define	ENOSR		63	/* Out of streams resources */
#define	ENONET		64	/* Machine is not on the network */
#define	ENOPKG		65	/* Package not installed */
#define	EREMOTE		66	/* Object is remote */
#define	ENOLINK		67	/* Link has been severed */
#define	EADV		68	/* Advertise error */
#define	ESRMNT		69	/* Srmount error */
#define	ECOMM		70	/* Communication error on send */
#define	EPROTO		71	/* Protocol error */
#define	EDOTDOT		73	/* RFS specific error */
#define	EMULTIHOP	74	/* Multihop attempted */
#define	EBADMSG		77	/* Not a data message */
#define	ENAMETOOLONG	78	/* File name too long */
#define	EOVERFLOW	79	/* Value too large for defined data type */
#define	ENOTUNIQ	80	/* Name not unique on network */
#define	EBADFD		81	/* File descriptor in bad state */
#define	EREMCHG		82	/* Remote address changed */
#define	ELIBACC		83	/* Can not access a needed shared library */
#define	ELIBBAD		84	/* Accessing a corrupted shared library */
#define	ELIBSCN		85	/* .lib section in a.out corrupted */
#define	ELIBMAX		86	/* Attempting to link in too many shared libraries */
#define	ELIBEXEC	87	/* Cannot exec a shared library directly */
#define	EILSEQ		88	/* Illegal byte sequence */
#define	ENOSYS		89	/* Function not implemented */
#define	ELOOP		90	/* Too many symbolic links encountered */
#define	ERESTART	91	/* Interrupted system call should be restarted */
#define	ESTRPIPE	92	/* Streams pipe error */
#define	ENOTEMPTY	93	/* Directory not empty */
#define	EUSERS		94	/* Too many users */
#define	ENOTSOCK	95	/* Socket operation on non-socket */
#define	EDESTADDRREQ	96	/* Destination address required */
#define	EMSGSIZE	97	/* Message too long */
#define	EPROTOTYPE	98	/* Protocol wrong type for socket */
#define	ENOPROTOOPT	99	/* Protocol not available */
#define	EPROTONOSUPPORT	120	/* Protocol not supported */
#define	ESOCKTNOSUPPORT	121	/* Socket type not supported */
#define	EOPNOTSUPP	122	/* Operation not supported on transport endpoint */
#define	EPFNOSUPPORT	123	/* Protocol family not supported */
#define	EAFNOSUPPORT	124	/* Address family not supported by protocol */
#define	EADDRINUSE	125	/* Address already in use */
#define	EADDRNOTAVAIL	126	/* Cannot assign requested address */
#define	ENETDOWN	127	/* Network is down */
#define	ENETUNREACH	128	/* Network is unreachable */
#define	ENETRESET	129	/* Network dropped connection because of reset */
#define	ECONNABORTED	130	/* Software caused connection abort */
#define	ECONNRESET	131	/* Connection reset by peer */
#define	ENOBUFS		132	/* No buffer space available */
#define	EISCONN		133	/* Transport endpoint is already connected */
#define	ENOTCONN	134	/* Transport endpoint is not connected */
#define	EUCLEAN		135	/* Structure needs cleaning */
#define	ENOTNAM		137	/* Not a XENIX named type file */
#define	ENAVAIL		138	/* No XENIX semaphores available */
#define	EISNAM		139	/* Is a named type file */
#define	EREMOTEIO	140	/* Remote I/O error */
#define EINIT		141	/* Reserved */
#define EREMDEV		142	/* Error 142 */
#define	ESHUTDOWN	143	/* Cannot send after transport endpoint shutdown */
#define	ETOOMANYREFS	144	/* Too many references: cannot splice */
#define	ETIMEDOUT	145	/* Connection timed out */
#define	ECONNREFUSED	146	/* Connection refused */
#define	EHOSTDOWN	147	/* Host is down */
#define	EHOSTUNREACH	148	/* No route to host */
#define	EWOULDBLOCK	EAGAIN	/* Operation would block */
#define	EALREADY	149	/* Operation already in progress */
#define	EINPROGRESS	150	/* Operation now in progress */
#define	ESTALE		151	/* Stale NFS file handle */
#define ECANCELED	158	/* AIO operation canceled */



/*
 *	Force alignment in a struct if required by processor
 */
union ALIGNMENT {
	u64 u64align;
	void *ptralign;
} ;

#endif /* defined _NTFS_TYPES_H */

