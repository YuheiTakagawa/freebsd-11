/*-
 * Copyright (c) 1982, 1986 The Regents of the University of California.
 * Copyright (c) 1989, 1990 William Jolitz
 * Copyright (c) 1994 John Dyson
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * the Systems Programming Group of the University of Utah Computer
 * Science Department, and William Jolitz.
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
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
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
 *	from: @(#)vm_machdep.c	7.3 (Berkeley) 5/13/91
 *	Utah $Hdr: vm_machdep.c 1.16.1.1 89/06/23$
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD: releng/11.0/sys/amd64/amd64/vm_machdep.c 301961 2016-06-16 12:05:44Z kib $");

#include "opt_isa.h"
#include "opt_cpu.h"
#include "opt_compat.h"

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/bio.h>
#include <sys/buf.h>
#include <sys/kernel.h>
#include <sys/ktr.h>
#include <sys/lock.h>
#include <sys/malloc.h>
#include <sys/mbuf.h>
#include <sys/mutex.h>
#include <sys/pioctl.h>
#include <sys/proc.h>
#include <sys/smp.h>
#include <sys/sysctl.h>
#include <sys/sysent.h>
#include <sys/unistd.h>
#include <sys/vnode.h>
#include <sys/vmmeter.h>

#include <machine/cpu.h>
#include <machine/md_var.h>
#include <machine/pcb.h>
#include <machine/smp.h>
#include <machine/specialreg.h>
#include <machine/tss.h>

#include <vm/vm.h>
#include <vm/vm_extern.h>
#include <vm/vm_kern.h>
#include <vm/vm_page.h>
#include <vm/vm_map.h>
#include <vm/vm_param.h>

#include <isa/isareg.h>

static void	cpu_reset_real(void);
#ifdef SMP
static void	cpu_reset_proxy(void);
static u_int	cpu_reset_proxyid;
static volatile u_int	cpu_reset_proxy_active;
#endif

_Static_assert(OFFSETOF_CURTHREAD == offsetof(struct pcpu, pc_curthread),
    "OFFSETOF_CURTHREAD does not correspond with offset of pc_curthread.");
_Static_assert(OFFSETOF_CURPCB == offsetof(struct pcpu, pc_curpcb),
    "OFFSETOF_CURPCB does not correspond with offset of pc_curpcb.");
_Static_assert(OFFSETOF_MONITORBUF == offsetof(struct pcpu, pc_monitorbuf),
    "OFFSETOF_MONINORBUF does not correspond with offset of pc_monitorbuf.");

struct savefpu *
get_pcb_user_save_td(struct thread *td)
{
	vm_offset_t p;

	p = td->td_kstack + td->td_kstack_pages * PAGE_SIZE -
	    roundup2(cpu_max_ext_state_size, XSAVE_AREA_ALIGN);
	KASSERT((p % XSAVE_AREA_ALIGN) == 0, ("Unaligned pcb_user_save area"));
	return ((struct savefpu *)p);
}

struct savefpu *
get_pcb_user_save_pcb(struct pcb *pcb)
{
	vm_offset_t p;

	p = (vm_offset_t)(pcb + 1);
	return ((struct savefpu *)p);
}

struct pcb *
get_pcb_td(struct thread *td)
{
	vm_offset_t p;

	p = td->td_kstack + td->td_kstack_pages * PAGE_SIZE -
	    roundup2(cpu_max_ext_state_size, XSAVE_AREA_ALIGN) -
	    sizeof(struct pcb);
	return ((struct pcb *)p);
}

void *
alloc_fpusave(int flags)
{
	void *res;
	struct savefpu_ymm *sf;

	res = malloc(cpu_max_ext_state_size, M_DEVBUF, flags);
	if (use_xsave) {
		sf = (struct savefpu_ymm *)res;
		bzero(&sf->sv_xstate.sx_hd, sizeof(sf->sv_xstate.sx_hd));
		sf->sv_xstate.sx_hd.xstate_bv = xsave_mask;
	}
	return (res);
}

/*
 * Finish a fork operation, with process p2 nearly set up.
 * Copy and update the pcb, set up the stack so that the child
 * ready to run and return to user mode.
 */
void
cpu_fork(td1, p2, td2, flags)
	register struct thread *td1;
	register struct proc *p2;
	struct thread *td2;
	int flags;
{
	register struct proc *p1;
	struct pcb *pcb2;
	struct mdproc *mdp1, *mdp2;
	struct proc_ldt *pldt;

	p1 = td1->td_proc;
	if ((flags & RFPROC) == 0) {
		if ((flags & RFMEM) == 0) {
			/* unshare user LDT */
			mdp1 = &p1->p_md;
			mtx_lock(&dt_lock);
			if ((pldt = mdp1->md_ldt) != NULL &&
			    pldt->ldt_refcnt > 1 &&
			    user_ldt_alloc(p1, 1) == NULL)
				panic("could not copy LDT");
			mtx_unlock(&dt_lock);
		}
		return;
	}

	/* Ensure that td1's pcb is up to date. */
	fpuexit(td1);

	/* Point the pcb to the top of the stack */
	pcb2 = get_pcb_td(td2);
	td2->td_pcb = pcb2;

	/* Copy td1's pcb */
	bcopy(td1->td_pcb, pcb2, sizeof(*pcb2));

	/* Properly initialize pcb_save */
	pcb2->pcb_save = get_pcb_user_save_pcb(pcb2);
	bcopy(get_pcb_user_save_td(td1), get_pcb_user_save_pcb(pcb2),
	    cpu_max_ext_state_size);

	/* Point mdproc and then copy over td1's contents */
	mdp2 = &p2->p_md;
	bcopy(&p1->p_md, mdp2, sizeof(*mdp2));

	/*
	 * Create a new fresh stack for the new process.
	 * Copy the trap frame for the return to user mode as if from a
	 * syscall.  This copies most of the user mode register values.
	 */
	td2->td_frame = (struct trapframe *)td2->td_pcb - 1;
	bcopy(td1->td_frame, td2->td_frame, sizeof(struct trapframe));

	td2->td_frame->tf_rax = 0;		/* Child returns zero */
	td2->td_frame->tf_rflags &= ~PSL_C;	/* success */
	td2->td_frame->tf_rdx = 1;

	/*
	 * If the parent process has the trap bit set (i.e. a debugger had
	 * single stepped the process to the system call), we need to clear
	 * the trap flag from the new frame unless the debugger had set PF_FORK
	 * on the parent.  Otherwise, the child will receive a (likely
	 * unexpected) SIGTRAP when it executes the first instruction after
	 * returning  to userland.
	 */
	if ((p1->p_pfsflags & PF_FORK) == 0)
		td2->td_frame->tf_rflags &= ~PSL_T;

	/*
	 * Set registers for trampoline to user mode.  Leave space for the
	 * return address on stack.  These are the kernel mode register values.
	 */
	pcb2->pcb_r12 = (register_t)fork_return;	/* fork_trampoline argument */
	pcb2->pcb_rbp = 0;
	pcb2->pcb_rsp = (register_t)td2->td_frame - sizeof(void *);
	pcb2->pcb_rbx = (register_t)td2;		/* fork_trampoline argument */
	pcb2->pcb_rip = (register_t)fork_trampoline;
	/*-
	 * pcb2->pcb_dr*:	cloned above.
	 * pcb2->pcb_savefpu:	cloned above.
	 * pcb2->pcb_flags:	cloned above.
	 * pcb2->pcb_onfault:	cloned above (always NULL here?).
	 * pcb2->pcb_[fg]sbase:	cloned above
	 */

	/* Setup to release spin count in fork_exit(). */
	td2->td_md.md_spinlock_count = 1;
	td2->td_md.md_saved_flags = PSL_KERNEL | PSL_I;
	td2->td_md.md_invl_gen.gen = 0;

	/* As an i386, do not copy io permission bitmap. */
	pcb2->pcb_tssp = NULL;

	/* New segment registers. */
	set_pcb_flags(pcb2, PCB_FULL_IRET);

	/* Copy the LDT, if necessary. */
	mdp1 = &td1->td_proc->p_md;
	mdp2 = &p2->p_md;
	mtx_lock(&dt_lock);
	if (mdp1->md_ldt != NULL) {
		if (flags & RFMEM) {
			mdp1->md_ldt->ldt_refcnt++;
			mdp2->md_ldt = mdp1->md_ldt;
			bcopy(&mdp1->md_ldt_sd, &mdp2->md_ldt_sd, sizeof(struct
			    system_segment_descriptor));
		} else {
			mdp2->md_ldt = NULL;
			mdp2->md_ldt = user_ldt_alloc(p2, 0);
			if (mdp2->md_ldt == NULL)
				panic("could not copy LDT");
			amd64_set_ldt_data(td2, 0, max_ldt_segment,
			    (struct user_segment_descriptor *)
			    mdp1->md_ldt->ldt_base);
		}
	} else
		mdp2->md_ldt = NULL;
	mtx_unlock(&dt_lock);

	/*
	 * Now, cpu_switch() can schedule the new process.
	 * pcb_rsp is loaded pointing to the cpu_switch() stack frame
	 * containing the return address when exiting cpu_switch.
	 * This will normally be to fork_trampoline(), which will have
	 * %ebx loaded with the new proc's pointer.  fork_trampoline()
	 * will set up a stack to call fork_return(p, frame); to complete
	 * the return to user-mode.
	 */
}

/*
 * Intercept the return address from a freshly forked process that has NOT
 * been scheduled yet.
 *
 * This is needed to make kernel threads stay in kernel mode.
 */
void
cpu_fork_kthread_handler(struct thread *td, void (*func)(void *), void *arg)
{
	/*
	 * Note that the trap frame follows the args, so the function
	 * is really called like this:  func(arg, frame);
	 */
	td->td_pcb->pcb_r12 = (long) func;	/* function */
	td->td_pcb->pcb_rbx = (long) arg;	/* first arg */
}

void
cpu_exit(struct thread *td)
{

	/*
	 * If this process has a custom LDT, release it.
	 */
	mtx_lock(&dt_lock);
	if (td->td_proc->p_md.md_ldt != 0)
		user_ldt_free(td);
	else
		mtx_unlock(&dt_lock);
}

void
cpu_thread_exit(struct thread *td)
{
	struct pcb *pcb;

	critical_enter();
	if (td == PCPU_GET(fpcurthread))
		fpudrop();
	critical_exit();

	pcb = td->td_pcb;

	/* Disable any hardware breakpoints. */
	if (pcb->pcb_flags & PCB_DBREGS) {
		reset_dbregs();
		clear_pcb_flags(pcb, PCB_DBREGS);
	}
}

void
cpu_thread_clean(struct thread *td)
{
	struct pcb *pcb;

	pcb = td->td_pcb;

	/*
	 * Clean TSS/iomap
	 */
	if (pcb->pcb_tssp != NULL) {
		kmem_free(kernel_arena, (vm_offset_t)pcb->pcb_tssp,
		    ctob(IOPAGES + 1));
		pcb->pcb_tssp = NULL;
	}
}

void
cpu_thread_swapin(struct thread *td)
{
}

void
cpu_thread_swapout(struct thread *td)
{
}

void
cpu_thread_alloc(struct thread *td)
{
	struct pcb *pcb;
	struct xstate_hdr *xhdr;

	td->td_pcb = pcb = get_pcb_td(td);
	td->td_frame = (struct trapframe *)pcb - 1;
	pcb->pcb_save = get_pcb_user_save_pcb(pcb);
	if (use_xsave) {
		xhdr = (struct xstate_hdr *)(pcb->pcb_save + 1);
		bzero(xhdr, sizeof(*xhdr));
		xhdr->xstate_bv = xsave_mask;
	}
}

void
cpu_thread_free(struct thread *td)
{

	cpu_thread_clean(td);
}

void
cpu_set_syscall_retval(struct thread *td, int error)
{

	switch (error) {
	case 0:
		td->td_frame->tf_rax = td->td_retval[0];
		td->td_frame->tf_rdx = td->td_retval[1];
		td->td_frame->tf_rflags &= ~PSL_C;
		break;

	case ERESTART:
		/*
		 * Reconstruct pc, we know that 'syscall' is 2 bytes,
		 * lcall $X,y is 7 bytes, int 0x80 is 2 bytes.
		 * We saved this in tf_err.
		 * %r10 (which was holding the value of %rcx) is restored
		 * for the next iteration.
		 * %r10 restore is only required for freebsd/amd64 processes,
		 * but shall be innocent for any ia32 ABI.
		 *
		 * Require full context restore to get the arguments
		 * in the registers reloaded at return to usermode.
		 */
		td->td_frame->tf_rip -= td->td_frame->tf_err;
		td->td_frame->tf_r10 = td->td_frame->tf_rcx;
		set_pcb_flags(td->td_pcb, PCB_FULL_IRET);
		break;

	case EJUSTRETURN:
		break;

	default:
		td->td_frame->tf_rax = SV_ABI_ERRNO(td->td_proc, error);
		td->td_frame->tf_rflags |= PSL_C;
		break;
	}
}

/*
 * Initialize machine state, mostly pcb and trap frame for a new
 * thread, about to return to userspace.  Put enough state in the new
 * thread's PCB to get it to go back to the fork_return(), which
 * finalizes the thread state and handles peculiarities of the first
 * return to userspace for the new thread.
 */
void
cpu_copy_thread(struct thread *td, struct thread *td0)
{
	struct pcb *pcb2;

	/* Point the pcb to the top of the stack. */
	pcb2 = td->td_pcb;

	/*
	 * Copy the upcall pcb.  This loads kernel regs.
	 * Those not loaded individually below get their default
	 * values here.
	 */
	bcopy(td0->td_pcb, pcb2, sizeof(*pcb2));
	clear_pcb_flags(pcb2, PCB_FPUINITDONE | PCB_USERFPUINITDONE |
	    PCB_KERNFPU);
	pcb2->pcb_save = get_pcb_user_save_pcb(pcb2);
	bcopy(get_pcb_user_save_td(td0), pcb2->pcb_save,
	    cpu_max_ext_state_size);
	set_pcb_flags(pcb2, PCB_FULL_IRET);

	/*
	 * Create a new fresh stack for the new thread.
	 */
	bcopy(td0->td_frame, td->td_frame, sizeof(struct trapframe));

	/* If the current thread has the trap bit set (i.e. a debugger had
	 * single stepped the process to the system call), we need to clear
	 * the trap flag from the new frame. Otherwise, the new thread will
	 * receive a (likely unexpected) SIGTRAP when it executes the first
	 * instruction after returning to userland.
	 */
	td->td_frame->tf_rflags &= ~PSL_T;

	/*
	 * Set registers for trampoline to user mode.  Leave space for the
	 * return address on stack.  These are the kernel mode register values.
	 */
	pcb2->pcb_r12 = (register_t)fork_return;	    /* trampoline arg */
	pcb2->pcb_rbp = 0;
	pcb2->pcb_rsp = (register_t)td->td_frame - sizeof(void *);	/* trampoline arg */
	pcb2->pcb_rbx = (register_t)td;			    /* trampoline arg */
	pcb2->pcb_rip = (register_t)fork_trampoline;
	/*
	 * If we didn't copy the pcb, we'd need to do the following registers:
	 * pcb2->pcb_dr*:	cloned above.
	 * pcb2->pcb_savefpu:	cloned above.
	 * pcb2->pcb_onfault:	cloned above (always NULL here?).
	 * pcb2->pcb_[fg]sbase: cloned above
	 */

	/* Setup to release spin count in fork_exit(). */
	td->td_md.md_spinlock_count = 1;
	td->td_md.md_saved_flags = PSL_KERNEL | PSL_I;
}

/*
 * Set that machine state for performing an upcall that starts
 * the entry function with the given argument.
 */
void
cpu_set_upcall(struct thread *td, void (*entry)(void *), void *arg,
    stack_t *stack)
{

	/* 
	 * Do any extra cleaning that needs to be done.
	 * The thread may have optional components
	 * that are not present in a fresh thread.
	 * This may be a recycled thread so make it look
	 * as though it's newly allocated.
	 */
	cpu_thread_clean(td);

#ifdef COMPAT_FREEBSD32
	if (SV_PROC_FLAG(td->td_proc, SV_ILP32)) {
		/*
		 * Set the trap frame to point at the beginning of the entry
		 * function.
		 */
		td->td_frame->tf_rbp = 0;
		td->td_frame->tf_rsp =
		   (((uintptr_t)stack->ss_sp + stack->ss_size - 4) & ~0x0f) - 4;
		td->td_frame->tf_rip = (uintptr_t)entry;

		/* Pass the argument to the entry point. */
		suword32((void *)(td->td_frame->tf_rsp + sizeof(int32_t)),
		    (uint32_t)(uintptr_t)arg);

		return;
	}
#endif

	/*
	 * Set the trap frame to point at the beginning of the uts
	 * function.
	 */
	td->td_frame->tf_rbp = 0;
	td->td_frame->tf_rsp =
	    ((register_t)stack->ss_sp + stack->ss_size) & ~0x0f;
	td->td_frame->tf_rsp -= 8;
	td->td_frame->tf_rip = (register_t)entry;
	td->td_frame->tf_ds = _udatasel;
	td->td_frame->tf_es = _udatasel;
	td->td_frame->tf_fs = _ufssel;
	td->td_frame->tf_gs = _ugssel;
	td->td_frame->tf_flags = TF_HASSEGS;

	/* Pass the argument to the entry point. */
	td->td_frame->tf_rdi = (register_t)arg;
}

int
cpu_set_user_tls(struct thread *td, void *tls_base)
{
	struct pcb *pcb;

	if ((u_int64_t)tls_base >= VM_MAXUSER_ADDRESS)
		return (EINVAL);

	pcb = td->td_pcb;
	set_pcb_flags(pcb, PCB_FULL_IRET);
#ifdef COMPAT_FREEBSD32
	if (SV_PROC_FLAG(td->td_proc, SV_ILP32)) {
		pcb->pcb_gsbase = (register_t)tls_base;
		return (0);
	}
#endif
	pcb->pcb_fsbase = (register_t)tls_base;
	return (0);
}

#ifdef SMP
static void
cpu_reset_proxy()
{
	cpuset_t tcrp;

	cpu_reset_proxy_active = 1;
	while (cpu_reset_proxy_active == 1)
		ia32_pause(); /* Wait for other cpu to see that we've started */

	CPU_SETOF(cpu_reset_proxyid, &tcrp);
	stop_cpus(tcrp);
	printf("cpu_reset_proxy: Stopped CPU %d\n", cpu_reset_proxyid);
	DELAY(1000000);
	cpu_reset_real();
}
#endif

void
cpu_reset()
{
#ifdef SMP
	cpuset_t map;
	u_int cnt;

	if (smp_started) {
		map = all_cpus;
		CPU_CLR(PCPU_GET(cpuid), &map);
		CPU_NAND(&map, &stopped_cpus);
		if (!CPU_EMPTY(&map)) {
			printf("cpu_reset: Stopping other CPUs\n");
			stop_cpus(map);
		}

		if (PCPU_GET(cpuid) != 0) {
			cpu_reset_proxyid = PCPU_GET(cpuid);
			cpustop_restartfunc = cpu_reset_proxy;
			cpu_reset_proxy_active = 0;
			printf("cpu_reset: Restarting BSP\n");

			/* Restart CPU #0. */
			CPU_SETOF(0, &started_cpus);
			wmb();

			cnt = 0;
			while (cpu_reset_proxy_active == 0 && cnt < 10000000) {
				ia32_pause();
				cnt++;	/* Wait for BSP to announce restart */
			}
			if (cpu_reset_proxy_active == 0)
				printf("cpu_reset: Failed to restart BSP\n");
			enable_intr();
			cpu_reset_proxy_active = 2;

			while (1)
				ia32_pause();
			/* NOTREACHED */
		}

		DELAY(1000000);
	}
#endif
	cpu_reset_real();
	/* NOTREACHED */
}

static void
cpu_reset_real()
{
	struct region_descriptor null_idt;
	int b;

	disable_intr();

	/*
	 * Attempt to do a CPU reset via the keyboard controller,
	 * do not turn off GateA20, as any machine that fails
	 * to do the reset here would then end up in no man's land.
	 */
	outb(IO_KBD + 4, 0xFE);
	DELAY(500000);	/* wait 0.5 sec to see if that did it */

	/*
	 * Attempt to force a reset via the Reset Control register at
	 * I/O port 0xcf9.  Bit 2 forces a system reset when it
	 * transitions from 0 to 1.  Bit 1 selects the type of reset
	 * to attempt: 0 selects a "soft" reset, and 1 selects a
	 * "hard" reset.  We try a "hard" reset.  The first write sets
	 * bit 1 to select a "hard" reset and clears bit 2.  The
	 * second write forces a 0 -> 1 transition in bit 2 to trigger
	 * a reset.
	 */
	outb(0xcf9, 0x2);
	outb(0xcf9, 0x6);
	DELAY(500000);  /* wait 0.5 sec to see if that did it */

	/*
	 * Attempt to force a reset via the Fast A20 and Init register
	 * at I/O port 0x92.  Bit 1 serves as an alternate A20 gate.
	 * Bit 0 asserts INIT# when set to 1.  We are careful to only
	 * preserve bit 1 while setting bit 0.  We also must clear bit
	 * 0 before setting it if it isn't already clear.
	 */
	b = inb(0x92);
	if (b != 0xff) {
		if ((b & 0x1) != 0)
			outb(0x92, b & 0xfe);
		outb(0x92, b | 0x1);
		DELAY(500000);  /* wait 0.5 sec to see if that did it */
	}

	printf("No known reset method worked, attempting CPU shutdown\n");
	DELAY(1000000);	/* wait 1 sec for printf to complete */

	/* Wipe the IDT. */
	null_idt.rd_limit = 0;
	null_idt.rd_base = 0;
	lidt(&null_idt);

	/* "good night, sweet prince .... <THUNK!>" */
	breakpoint();

	/* NOTREACHED */
	while(1);
}

/*
 * Software interrupt handler for queued VM system processing.
 */   
void  
swi_vm(void *dummy) 
{     
	if (busdma_swi_pending != 0)
		busdma_swi();
}

/*
 * Tell whether this address is in some physical memory region.
 * Currently used by the kernel coredump code in order to avoid
 * dumping the ``ISA memory hole'' which could cause indefinite hangs,
 * or other unpredictable behaviour.
 */

int
is_physical_memory(vm_paddr_t addr)
{

#ifdef DEV_ISA
	/* The ISA ``memory hole''. */
	if (addr >= 0xa0000 && addr < 0x100000)
		return 0;
#endif

	/*
	 * stuff other tests for known memory-mapped devices (PCI?)
	 * here
	 */

	return 1;
}
