/*
 * chap.h - Challenge Handshake Authentication Protocol definitions.
 *
 * Copyright (c) 1995 Eric Rosenquist, Strata Software Limited.
 * http://www.strataware.com/
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by Eric Rosenquist.  The name of the author may not be used to
 * endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * $Id: chap_ms.h,v 1.1 2010/01/28 11:09:57 dennis Exp $
 */

#ifndef __CHAPMS_INCLUDE__

#ifndef __P
# if defined(__STDC__) || defined(__GNUC__)
#  define __P(x) x
# else
#  define __P(x) ()
# endif
#endif

void ChapMS __P((chap_state *, char *, int, char *, int));
int  ChapMS_Resp __P((chap_state *, char *, int, u_char *));
void ChapMS_v2 __P((chap_state *, char *, int, char *, int));
int  ChapMS_v2_Resp __P((chap_state *, char *, int, u_char *, char *));
void ChapMS_v2_Auth __P((chap_state *, char *, int, u_char *, char *));

#define __CHAPMS_INCLUDE__
#endif /* __CHAPMS_INCLUDE__ */


