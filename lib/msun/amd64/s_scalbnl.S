/*
 * Based on code written by J.T. Conklin <jtc@netbsd.org>.
 * Public domain.
 */

#include <machine/asm.h>
__FBSDID("$FreeBSD: releng/11.0/lib/msun/amd64/s_scalbnl.S 217108 2011-01-07 16:13:12Z kib $")
/* RCSID("$NetBSD: s_scalbnf.S,v 1.4 1999/01/02 05:15:40 kristerw Exp $") */

ENTRY(scalbnl)
	movl	%edi,-4(%rsp)
	fildl	-4(%rsp)
	fldt	8(%rsp)
	fscale
	fstp	%st(1)
	ret
END(scalbnl)

.globl	CNAME(ldexpl)
.set	CNAME(ldexpl),CNAME(scalbnl)

	.section .note.GNU-stack,"",%progbits
