#	$NetBSD: Makefile,v 1.3 1995/07/04 01:53:05 cgd Exp $
# $FreeBSD: releng/11.0/usr.bin/xlint/lint1/Makefile 291348 2015-11-26 01:14:40Z bdrewery $

PROG=	lint1
SRCS=	cgram.y scan.l mem1.c mem.c err.c main1.c decl.c tree.c func.c \
	init.c emit.c emit1.c inittyp.c
MAN=	lint.7
CLEANFILES= lint.7

LIBADD=	l m
CFLAGS+= -I. -I${.CURDIR}
LINTFLAGS=-aehpz

BINDIR=	${LIBEXECDIR}

.PATH: ${.CURDIR}/../common

lint.7: makeman
	sh ${.CURDIR}/makeman ${DESTDIR}${BINDIR}/${PROG} -m >${.TARGET}

.include <bsd.prog.mk>
