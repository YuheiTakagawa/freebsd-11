/*
 * Linker script for 32-bit vDSO.
 * Copied from Linux kernel arch/x86/vdso/vdso-layout.lds.S
 * and arch/x86/vdso/vdso32/vdso32.lds.S
 *
 * $FreeBSD: releng/11.0/sys/i386/linux/linux_vdso.lds.s 283407 2015-05-24 15:28:17Z dchagin $
 */

SECTIONS
{
	. = . + SIZEOF_HEADERS;

	.hash		: { *(.hash) }			:text
	.gnu.hash	: { *(.gnu.hash) }
	.dynsym		: { *(.dynsym) }
	.dynstr		: { *(.dynstr) }
	.gnu.version	: { *(.gnu.version) }
	.gnu.version_d	: { *(.gnu.version_d) }
	.gnu.version_r	: { *(.gnu.version_r) }

	.note		: { *(.note.*) }		:text	:note

	.eh_frame_hdr	: { *(.eh_frame_hdr) }		:text	:eh_frame_hdr
	.eh_frame	: { KEEP (*(.eh_frame)) }	:text

	.dynamic	: { *(.dynamic) }		:text	:dynamic

	.rodata		: { *(.rodata*) }		:text
	.data		: {
	      *(.data*)
	      *(.sdata*)
	      *(.got.plt) *(.got)
	      *(.gnu.linkonce.d.*)
	      *(.bss*)
	      *(.dynbss*)
	      *(.gnu.linkonce.b.*)
	}

	.altinstructions	: { *(.altinstructions) }
	.altinstr_replacement	: { *(.altinstr_replacement) }

	. = ALIGN(0x100);
	.text		: { *(.text*) }			:text	=0x90909090
}

PHDRS
{
	text		PT_LOAD		FLAGS(5) FILEHDR PHDRS; /* PF_R|PF_X */
	dynamic		PT_DYNAMIC	FLAGS(4);		/* PF_R */
	note		PT_NOTE		FLAGS(4);		/* PF_R */
	eh_frame_hdr	PT_GNU_EH_FRAME;
}

ENTRY(linux_vsyscall);

VERSION
{
	LINUX_2.5 {
	global:
		linux_vsyscall;
		linux_sigcode;
		linux_rt_sigcode;
	local: *;
	};
}