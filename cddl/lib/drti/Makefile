# $FreeBSD: releng/11.0/cddl/lib/drti/Makefile 291729 2015-12-04 00:29:40Z bdrewery $

.PATH:		${.CURDIR}/../../../cddl/contrib/opensolaris/lib/libdtrace/common

SRCS=		drti.c
FILES=		${SRCS:R:S/$/.o/g}
FILESOWN=	${LIBOWN}
FILESGRP=	${LIBGRP}
FILESMODE=	${LIBMODE}
FILESDIR=	${LIBDIR}/dtrace
CLEANFILES=	${FILES}
# These FILES qualify as libraries for the purpose of LIBRARIES_ONLY.
.undef LIBRARIES_ONLY

CFLAGS+=	-I${.CURDIR}/../../../sys/cddl/compat/opensolaris \
		-I${.CURDIR}/../../../cddl/compat/opensolaris/include \
		-I${OPENSOLARIS_USR_DISTDIR}/head \
		-I${OPENSOLARIS_USR_DISTDIR}/lib/libctf/common \
		-I${OPENSOLARIS_USR_DISTDIR}/lib/libdtrace/common \
		-I${OPENSOLARIS_SYS_DISTDIR}/uts/common \
		-DPIC ${PICFLAG}

.include <bsd.lib.mk>
