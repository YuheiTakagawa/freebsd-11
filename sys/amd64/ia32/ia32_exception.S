/*-
 * Copyright (c) 2003 Peter Wemm
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
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD: releng/11.0/sys/amd64/ia32/ia32_exception.S 220451 2011-04-08 13:30:48Z jhb $
 */

#include <machine/asmacros.h>

#include "assym.s"

	.text
/*
 * Call gate entry for FreeBSD ELF and Linux/NetBSD syscall (int 0x80)
 *
 * This is a SDT_SYSIDT entry point (unlike the i386 port) so that we
 * can do a swapgs before enabling interrupts.  This is critical because
 * if we took an interrupt before swapgs, the interrupt code would see
 * that it originated in supervisor mode and skip the swapgs.
 */
	SUPERALIGN_TEXT
IDTVEC(int0x80_syscall)
	swapgs
	pushq	$2			/* sizeof "int 0x80" */
	subq	$TF_ERR,%rsp		/* skip over tf_trapno */
	movq	%rdi,TF_RDI(%rsp)
	movq	PCPU(CURPCB),%rdi
	andl	$~PCB_FULL_IRET,PCB_FLAGS(%rdi)
	movw	%fs,TF_FS(%rsp)
	movw	%gs,TF_GS(%rsp)
	movw	%es,TF_ES(%rsp)
	movw	%ds,TF_DS(%rsp)
	sti
	movq	%rsi,TF_RSI(%rsp)
	movq	%rdx,TF_RDX(%rsp)
	movq	%rcx,TF_RCX(%rsp)
	movq	%r8,TF_R8(%rsp)
	movq	%r9,TF_R9(%rsp)
	movq	%rax,TF_RAX(%rsp)
	movq	%rbx,TF_RBX(%rsp)
	movq	%rbp,TF_RBP(%rsp)
	movq	%r10,TF_R10(%rsp)
	movq	%r11,TF_R11(%rsp)
	movq	%r12,TF_R12(%rsp)
	movq	%r13,TF_R13(%rsp)
	movq	%r14,TF_R14(%rsp)
	movq	%r15,TF_R15(%rsp)
	movl	$TF_HASSEGS,TF_FLAGS(%rsp)
	cld
	FAKE_MCOUNT(TF_RIP(%rsp))
	movq	%rsp, %rdi
	call	ia32_syscall
	MEXITCOUNT
	jmp	doreti
