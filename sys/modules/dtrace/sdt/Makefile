# $FreeBSD: releng/11.0/sys/modules/dtrace/sdt/Makefile 262818 2014-03-06 01:59:13Z marcel $

SYSDIR?=	${.CURDIR}/../../..

.PATH:		${SYSDIR}/cddl/dev/sdt

KMOD=		sdt
SRCS=		sdt.c
SRCS+=		vnode_if.h

CFLAGS+=	-I${SYSDIR}/cddl/compat/opensolaris \
		-I${SYSDIR}/cddl/contrib/opensolaris/uts/common \
		-I${SYSDIR}

.include <bsd.kmod.mk>

CFLAGS+=	-include ${SYSDIR}/cddl/compat/opensolaris/sys/debug_compat.h
