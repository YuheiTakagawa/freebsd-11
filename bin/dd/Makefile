#	@(#)Makefile	8.1 (Berkeley) 5/31/93
# $FreeBSD: releng/11.0/bin/dd/Makefile 298107 2016-04-16 07:45:30Z gjb $

.include <src.opts.mk>

PACKAGE=runtime
PROG=	dd
SRCS=	args.c conv.c conv_tab.c dd.c misc.c position.c

#
# Test the character conversion functions.  We have to be explicit about
# which LC_LANG we use because the definition of upper and lower case
# depends on it.
#

CLEANFILES=	gen

test: ${PROG} gen
.for conv in ascii ebcdic ibm oldascii oldebcdic oldibm \
    pareven parnone parodd parset \
    swab lcase ucase
	@${ECHO} testing conv=${conv}
	@./gen | \
	    LC_ALL=en_US.US-ASCII ./dd conv=${conv} 2>/dev/null | \
	    LC_ALL=en_US.US-ASCII hexdump -C | \
	    diff -I FreeBSD - ${.CURDIR}/ref.${conv}
.endfor
	@${ECHO} "testing sparse file (obs zeroes)"
	@./gen 189284 | ./dd ibs=16 obs=8 conv=sparse of=obs_zeroes 2> /dev/null
	@hexdump -C obs_zeroes | diff -I FreeBSD - ${.CURDIR}/ref.obs_zeroes

	@${ECHO} "testing spase file (all zeroes)"
	@./dd if=/dev/zero of=1M_zeroes bs=1048576 count=1 2> /dev/null
	@./dd if=1M_zeroes of=1M_zeroes.1 bs=1048576 conv=sparse 2> /dev/null
	@./dd if=1M_zeroes of=1M_zeroes.2 bs=1048576 2> /dev/null
	@diff 1M_zeroes 1M_zeroes.1
	@diff 1M_zeroes 1M_zeroes.2

	@rm -f gen 1M_zeroes* obs_zeroes

.if ${MK_TESTS} != "no"
SUBDIR+=	tests
.endif

.include <bsd.prog.mk>
