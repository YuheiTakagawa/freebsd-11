# $FreeBSD: releng/11.0/sbin/geom/Makefile 298107 2016-04-16 07:45:30Z gjb $

PACKAGE=runtime
.if defined(RESCUE) || defined(RELEASE_CRUNCH)

.PATH:	${.CURDIR}/class/part \
	${.CURDIR}/class/label \
	${.CURDIR}/core \
	${.CURDIR}/misc

PROG=	geom
SRCS=	geom.c geom_label.c geom_part.c subr.c
MAN=

WARNS?=	2
CFLAGS+=-I${.CURDIR} -I${.CURDIR}/core -DSTATIC_GEOM_CLASSES

LIBADD=	geom util

.include <bsd.prog.mk>

.else

SUBDIR=	core class

.include <bsd.subdir.mk>

.endif
