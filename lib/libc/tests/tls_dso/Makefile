# $FreeBSD: releng/11.0/lib/libc/tests/tls_dso/Makefile 299094 2016-05-04 23:20:53Z ngie $

.include <bsd.own.mk>

LIB=		h_tls_dynamic
SRCS=		h_tls_dynamic.c

LIBDIR=		${TESTSBASE}/lib/libc/tls
SHLIBDIR=	${TESTSBASE}/lib/libc/tls
SHLIB_MAJOR=	1

WITHOUT_STATIC=
WITHOUT_PROFILE=
WITHOUT_PIC=

MAN=

.include "../Makefile.netbsd-tests"

.include <bsd.lib.mk>
