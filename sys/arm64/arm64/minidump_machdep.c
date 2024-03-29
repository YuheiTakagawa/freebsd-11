/*-
 * Copyright (c) 2006 Peter Wemm
 * Copyright (c) 2015 The FreeBSD Foundation
 * All rights reserved.
 *
 * This software was developed by Andrew Turner under
 * sponsorship from the FreeBSD Foundation.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
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
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD: releng/11.0/sys/arm64/arm64/minidump_machdep.c 297446 2016-03-31 11:07:24Z andrew $");

#include "opt_watchdog.h"

#include "opt_watchdog.h"

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/conf.h>
#include <sys/cons.h>
#include <sys/kernel.h>
#include <sys/kerneldump.h>
#include <sys/msgbuf.h>
#include <sys/watchdog.h>

#include <vm/vm.h>
#include <vm/vm_param.h>
#include <vm/vm_page.h>
#include <vm/vm_phys.h>
#include <vm/pmap.h>

#include <machine/md_var.h>
#include <machine/pte.h>
#include <machine/minidump.h>

CTASSERT(sizeof(struct kerneldumpheader) == 512);

/*
 * Don't touch the first SIZEOF_METADATA bytes on the dump device. This
 * is to protect us from metadata and to protect metadata from us.
 */
#define	SIZEOF_METADATA		(64*1024)

uint64_t *vm_page_dump;
int vm_page_dump_size;

static struct kerneldumpheader kdh;
static off_t dumplo;

/* Handle chunked writes. */
static size_t fragsz;
static void *dump_va;
static size_t counter, progress, dumpsize;

static uint64_t tmpbuffer[PAGE_SIZE / sizeof(uint64_t)];

CTASSERT(sizeof(*vm_page_dump) == 8);

static int
is_dumpable(vm_paddr_t pa)
{
	vm_page_t m;
	int i;

	if ((m = vm_phys_paddr_to_vm_page(pa)) != NULL)
		return ((m->flags & PG_NODUMP) == 0);
	for (i = 0; dump_avail[i] != 0 || dump_avail[i + 1] != 0; i += 2) {
		if (pa >= dump_avail[i] && pa < dump_avail[i + 1])
			return (1);
	}
	return (0);
}

static int
blk_flush(struct dumperinfo *di)
{
	int error;

	if (fragsz == 0)
		return (0);

	error = dump_write(di, dump_va, 0, dumplo, fragsz);
	dumplo += fragsz;
	fragsz = 0;
	return (error);
}

static struct {
	int min_per;
	int max_per;
	int visited;
} progress_track[10] = {
	{  0,  10, 0},
	{ 10,  20, 0},
	{ 20,  30, 0},
	{ 30,  40, 0},
	{ 40,  50, 0},
	{ 50,  60, 0},
	{ 60,  70, 0},
	{ 70,  80, 0},
	{ 80,  90, 0},
	{ 90, 100, 0}
};

static void
report_progress(size_t progress, size_t dumpsize)
{
	int sofar, i;

	sofar = 100 - ((progress * 100) / dumpsize);
	for (i = 0; i < nitems(progress_track); i++) {
		if (sofar < progress_track[i].min_per ||
		    sofar > progress_track[i].max_per)
			continue;
		if (progress_track[i].visited)
			return;
		progress_track[i].visited = 1;
		printf("..%d%%", sofar);
		return;
	}
}

static int
blk_write(struct dumperinfo *di, char *ptr, vm_paddr_t pa, size_t sz)
{
	size_t len;
	int error, c;
	u_int maxdumpsz;

	maxdumpsz = min(di->maxiosize, MAXDUMPPGS * PAGE_SIZE);
	if (maxdumpsz == 0)	/* seatbelt */
		maxdumpsz = PAGE_SIZE;
	error = 0;
	if ((sz % PAGE_SIZE) != 0) {
		printf("size not page aligned\n");
		return (EINVAL);
	}
	if (ptr != NULL && pa != 0) {
		printf("cant have both va and pa!\n");
		return (EINVAL);
	}
	if ((((uintptr_t)pa) % PAGE_SIZE) != 0) {
		printf("address not page aligned %p\n", ptr);
		return (EINVAL);
	}
	if (ptr != NULL) {
		/*
		 * If we're doing a virtual dump, flush any
		 * pre-existing pa pages.
		 */
		error = blk_flush(di);
		if (error)
			return (error);
	}
	while (sz) {
		len = maxdumpsz - fragsz;
		if (len > sz)
			len = sz;
		counter += len;
		progress -= len;
		if (counter >> 22) {
			report_progress(progress, dumpsize);
			counter &= (1 << 22) - 1;
		}

		wdog_kern_pat(WD_LASTVAL);

		if (ptr) {
			error = dump_write(di, ptr, 0, dumplo, len);
			if (error)
				return (error);
			dumplo += len;
			ptr += len;
			sz -= len;
		} else {
			dump_va = (void *)PHYS_TO_DMAP(pa);
			fragsz += len;
			pa += len;
			sz -= len;
			error = blk_flush(di);
			if (error)
				return (error);
		}

		/* Check for user abort. */
		c = cncheckc();
		if (c == 0x03)
			return (ECANCELED);
		if (c != -1)
			printf(" (CTRL-C to abort) ");
	}

	return (0);
}

int
minidumpsys(struct dumperinfo *di)
{
	pd_entry_t *l0, *l1, *l2;
	pt_entry_t *l3;
	uint32_t pmapsize;
	vm_offset_t va;
	vm_paddr_t pa;
	int error;
	uint64_t bits;
	int i, bit;
	int retry_count;
	struct minidumphdr mdhdr;

	retry_count = 0;
 retry:
	retry_count++;
	error = 0;
	pmapsize = 0;
	for (va = VM_MIN_KERNEL_ADDRESS; va < kernel_vm_end; va += L2_SIZE) {
		pmapsize += PAGE_SIZE;
		if (!pmap_get_tables(pmap_kernel(), va, &l0, &l1, &l2, &l3))
			continue;

		/* We should always be using the l2 table for kvm */
		if (l2 == NULL)
			continue;

		if ((*l2 & ATTR_DESCR_MASK) == L2_BLOCK) {
			pa = *l2 & ~ATTR_MASK;
			for (i = 0; i < Ln_ENTRIES; i++, pa += PAGE_SIZE) {
				if (is_dumpable(pa))
					dump_add_page(pa);
			}
		} else if ((*l2 & ATTR_DESCR_MASK) == L2_TABLE) {
			for (i = 0; i < Ln_ENTRIES; i++) {
				if ((l3[i] & ATTR_DESCR_MASK) != L3_PAGE)
					continue;
				pa = l3[i] & ~ATTR_MASK;
				if (is_dumpable(pa))
					dump_add_page(pa);
			}
		}
	}

	/* Calculate dump size. */
	dumpsize = pmapsize;
	dumpsize += round_page(msgbufp->msg_size);
	dumpsize += round_page(vm_page_dump_size);
	for (i = 0; i < vm_page_dump_size / sizeof(*vm_page_dump); i++) {
		bits = vm_page_dump[i];
		while (bits) {
			bit = ffsl(bits) - 1;
			pa = (((uint64_t)i * sizeof(*vm_page_dump) * NBBY) +
			    bit) * PAGE_SIZE;
			/* Clear out undumpable pages now if needed */
			if (is_dumpable(pa))
				dumpsize += PAGE_SIZE;
			else
				dump_drop_page(pa);
			bits &= ~(1ul << bit);
		}
	}
	dumpsize += PAGE_SIZE;

	/* Determine dump offset on device. */
	if (di->mediasize < SIZEOF_METADATA + dumpsize + sizeof(kdh) * 2) {
		error = E2BIG;
		goto fail;
	}
	dumplo = di->mediaoffset + di->mediasize - dumpsize;
	dumplo -= sizeof(kdh) * 2;
	progress = dumpsize;

	/* Initialize mdhdr */
	bzero(&mdhdr, sizeof(mdhdr));
	strcpy(mdhdr.magic, MINIDUMP_MAGIC);
	mdhdr.version = MINIDUMP_VERSION;
	mdhdr.msgbufsize = msgbufp->msg_size;
	mdhdr.bitmapsize = vm_page_dump_size;
	mdhdr.pmapsize = pmapsize;
	mdhdr.kernbase = VM_MIN_KERNEL_ADDRESS;
	mdhdr.dmapphys = DMAP_MIN_PHYSADDR;
	mdhdr.dmapbase = DMAP_MIN_ADDRESS;
	mdhdr.dmapend = DMAP_MAX_ADDRESS;

	mkdumpheader(&kdh, KERNELDUMPMAGIC, KERNELDUMP_AARCH64_VERSION,
	    dumpsize, di->blocksize);

	printf("Dumping %llu out of %ju MB:", (long long)dumpsize >> 20,
	    ptoa((uintmax_t)physmem) / 1048576);

	/* Dump leader */
	error = dump_write(di, &kdh, 0, dumplo, sizeof(kdh));
	if (error)
		goto fail;
	dumplo += sizeof(kdh);

	/* Dump my header */
	bzero(&tmpbuffer, sizeof(tmpbuffer));
	bcopy(&mdhdr, &tmpbuffer, sizeof(mdhdr));
	error = blk_write(di, (char *)&tmpbuffer, 0, PAGE_SIZE);
	if (error)
		goto fail;

	/* Dump msgbuf up front */
	error = blk_write(di, (char *)msgbufp->msg_ptr, 0,
	    round_page(msgbufp->msg_size));
	if (error)
		goto fail;

	/* Dump bitmap */
	error = blk_write(di, (char *)vm_page_dump, 0,
	    round_page(vm_page_dump_size));
	if (error)
		goto fail;

	/* Dump kernel page directory pages */
	bzero(&tmpbuffer, sizeof(tmpbuffer));
	for (va = VM_MIN_KERNEL_ADDRESS; va < kernel_vm_end; va += L2_SIZE) {
		if (!pmap_get_tables(pmap_kernel(), va, &l0, &l1, &l2, &l3)) {
			/* We always write a page, even if it is zero */
			error = blk_write(di, (char *)&tmpbuffer, 0, PAGE_SIZE);
			if (error)
				goto fail;
			/* flush, in case we reuse tmpbuffer in the same block*/
			error = blk_flush(di);
			if (error)
				goto fail;
		} else if (l2 == NULL) {
			pa = (*l1 & ~ATTR_MASK) | (va & L1_OFFSET);

			/* Generate fake l3 entries based upon the l1 entry */
			for (i = 0; i < Ln_ENTRIES; i++) {
				tmpbuffer[i] = pa + (i * PAGE_SIZE) |
				    ATTR_DEFAULT | L3_PAGE;
			}
			/* We always write a page, even if it is zero */
			error = blk_write(di, (char *)&tmpbuffer, 0, PAGE_SIZE);
			if (error)
				goto fail;
			/* flush, in case we reuse tmpbuffer in the same block*/
			error = blk_flush(di);
			if (error)
				goto fail;
			bzero(&tmpbuffer, sizeof(tmpbuffer));
		} else if ((*l2 & ATTR_DESCR_MASK) == L2_BLOCK) {
			/* TODO: Handle an invalid L2 entry */
			pa = (*l2 & ~ATTR_MASK) | (va & L2_OFFSET);

			/* Generate fake l3 entries based upon the l1 entry */
			for (i = 0; i < Ln_ENTRIES; i++) {
				tmpbuffer[i] = pa + (i * PAGE_SIZE) |
				    ATTR_DEFAULT | L3_PAGE;
			}
			/* We always write a page, even if it is zero */
			error = blk_write(di, (char *)&tmpbuffer, 0, PAGE_SIZE);
			if (error)
				goto fail;
			/* flush, in case we reuse fakepd in the same block */
			error = blk_flush(di);
			if (error)
				goto fail;
			bzero(&tmpbuffer, sizeof(tmpbuffer));
			continue;
		} else {
			pa = *l2 & ~ATTR_MASK;

			/* We always write a page, even if it is zero */
			error = blk_write(di, NULL, pa, PAGE_SIZE);
			if (error)
				goto fail;
		}
	}

	/* Dump memory chunks */
	/* XXX cluster it up and use blk_dump() */
	for (i = 0; i < vm_page_dump_size / sizeof(*vm_page_dump); i++) {
		bits = vm_page_dump[i];
		while (bits) {
			bit = ffsl(bits) - 1;
			pa = (((uint64_t)i * sizeof(*vm_page_dump) * NBBY) +
			    bit) * PAGE_SIZE;
			error = blk_write(di, 0, pa, PAGE_SIZE);
			if (error)
				goto fail;
			bits &= ~(1ul << bit);
		}
	}

	error = blk_flush(di);
	if (error)
		goto fail;

	/* Dump trailer */
	error = dump_write(di, &kdh, 0, dumplo, sizeof(kdh));
	if (error)
		goto fail;
	dumplo += sizeof(kdh);

	/* Signal completion, signoff and exit stage left. */
	dump_write(di, NULL, 0, 0, 0);
	printf("\nDump complete\n");
	return (0);

 fail:
	if (error < 0)
		error = -error;

	printf("\n");
	if (error == ENOSPC) {
		printf("Dump map grown while dumping. ");
		if (retry_count < 5) {
			printf("Retrying...\n");
			goto retry;
		}
		printf("Dump failed.\n");
	}
	else if (error == ECANCELED)
		printf("Dump aborted\n");
	else if (error == E2BIG)
		printf("Dump failed. Partition too small.\n");
	else
		printf("** DUMP FAILED (ERROR %d) **\n", error);
	return (error);
}

void
dump_add_page(vm_paddr_t pa)
{
	int idx, bit;

	pa >>= PAGE_SHIFT;
	idx = pa >> 6;		/* 2^6 = 64 */
	bit = pa & 63;
	atomic_set_long(&vm_page_dump[idx], 1ul << bit);
}

void
dump_drop_page(vm_paddr_t pa)
{
	int idx, bit;

	pa >>= PAGE_SHIFT;
	idx = pa >> 6;		/* 2^6 = 64 */
	bit = pa & 63;
	atomic_clear_long(&vm_page_dump[idx], 1ul << bit);
}
