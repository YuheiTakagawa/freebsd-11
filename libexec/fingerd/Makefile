#	@(#)Makefile	8.1 (Berkeley) 6/4/93
# $FreeBSD: releng/11.0/libexec/fingerd/Makefile 301243 2016-06-03 07:00:28Z lidl $

.include <src.opts.mk>

PROG=	fingerd
LIBADD=	util
MAN=	fingerd.8

WARNS?=	2
WFORMAT=0

.if ${MK_BLACKLIST_SUPPORT} != "no"
CFLAGS+= -DUSE_BLACKLIST -I${SRCTOP}/contrib/blacklist/include
LIBADD+= blacklist
LDFLAGS+=-L${LIBBLACKLISTDIR}
.endif

.include <bsd.prog.mk>
