# $FreeBSD: releng/11.0/sbin/mount_udf/Makefile 298107 2016-04-16 07:45:30Z gjb $

PACKAGE=runtime
PROG=	mount_udf
SRCS=	mount_udf.c getmntopts.c
MAN=	mount_udf.8
LIBADD=	kiconv

MOUNT=	${.CURDIR}/../mount
CFLAGS+= -I${MOUNT} -I${.CURDIR}/../../sys
.PATH:	${MOUNT}

# Needs to be dynamically linked for optional dlopen() access to
# userland libiconv
NO_SHARED?=	NO

.include <bsd.prog.mk>
