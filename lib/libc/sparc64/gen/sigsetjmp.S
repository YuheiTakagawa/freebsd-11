/*
 * Copyright (c) 1995 Paul Kranenburg
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Paul Kranenburg.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <machine/asm.h>
__FBSDID("$FreeBSD: releng/11.0/lib/libc/sparc64/gen/sigsetjmp.S 108736 2003-01-05 22:10:06Z tmm $");

#include "assym.s"

ENTRY(sigsetjmp)
	PIC_PROLOGUE(%o3, %o2)
	SET(CNAME(setjmp), %o2, %o3)
	SET(CNAME(_setjmp), %o2, %o4)
	movrnz	%o1, %o3, %o4
	jmp	%o4
	 stx	%o1, [%o0 + _JB_SIGFLAG]
END(sigsetjmp)

	.weak CNAME(siglongjmp);
	.set CNAME(siglongjmp),CNAME(__siglongjmp);
ENTRY(__siglongjmp)
	PIC_PROLOGUE(%o3, %o2)
	SET(CNAME(longjmp), %o2, %o3)
	SET(CNAME(_longjmp), %o2, %o4)
	ldx	[%o0 + _JB_SIGFLAG], %o2
	movrnz	%o2, %o3, %o4
	jmp	%o4
	 nop
END(__siglongjmp)
