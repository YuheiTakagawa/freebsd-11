#	@(#)Makefile	8.1 (Berkeley) 6/5/93
# $FreeBSD: releng/11.0/sbin/dump/Makefile 298107 2016-04-16 07:45:30Z gjb $

#	dump.h			header file
#	itime.c			reads /etc/dumpdates
#	main.c			driver
#	optr.c			operator interface
#	dumprmt.c		handles remote tape via rmt(8)
#	tape.c			handles the mag tape and opening/closing
#	traverse.c		traverses the file system
#	unctime.c		undo ctime
#
#	DEBUG			use local directory to find ddate and dumpdates
#	TDEBUG			trace out the process forking

PACKAGE=runtime
PROG=	dump
LINKS=	${BINDIR}/dump ${BINDIR}/rdump
CFLAGS+=-DRDUMP
SRCS=	itime.c main.c optr.c dumprmt.c tape.c traverse.c unctime.c cache.c
MAN=	dump.8
MLINKS=	dump.8 rdump.8
WARNS?=	2

.include <bsd.prog.mk>
