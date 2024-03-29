/*
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This software was developed by the Computer Systems Engineering group
 * at Lawrence Berkeley Laboratory under DARPA contract BG 91-66 and
 * contributed to Berkeley.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * from: Header: setlogin.s,v 1.1 91/07/06 13:06:00 torek Exp
 */

#if defined(SYSLIBC_SCCS) && !defined(lint)
	.asciz "@(#)setlogin.s	8.1 (Berkeley) 6/4/93"
#if 0
	RCSID("$NetBSD: setlogin.S,v 1.3 2000/07/21 03:14:15 eeh Exp $")
#endif
#endif /* SYSLIBC_SCCS and not lint */
#include <machine/asm.h>
__FBSDID("$FreeBSD: releng/11.0/lib/libc/sparc64/sys/setlogin.S 165903 2007-01-09 00:28:16Z imp $");

#include "SYS.h"

	.globl	CNAME(_logname_valid)	/* in _getlogin() */

_SYSENTRY(setlogin)
	_SYSCALL(setlogin)
	PIC_PROLOGUE(%o3, %o2)
	SET(CNAME(_logname_valid), %o2, %o3)
	retl
	 stw	%g0, [%o3]
_SYSEND(setlogin)
