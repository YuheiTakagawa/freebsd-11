# $FreeBSD: releng/11.0/tools/build/Makefile 301763 2016-06-09 21:57:34Z jilles $

.PATH: ${.CURDIR}/../../include

LIB=		egacy
SRC=
INCSGROUPS=	INCS SYSINCS
INCS=

SYSINCSDIR=	${INCLUDEDIR}/sys

BOOTSTRAPPING?=	0

_WITH_PWCACHEDB!= grep -c pwcache_groupdb /usr/include/grp.h || true
.if ${_WITH_PWCACHEDB} == 0
.PATH: ${.CURDIR}/../../contrib/libc-pwcache
CFLAGS+=	-I${.CURDIR}/../../contrib/libc-pwcache \
		-I${.CURDIR}/../../lib/libc/include
SRCS+=		pwcache.c
.endif

_WITH_STRSVIS!=	grep -c strsvis /usr/include/vis.h || true
.if ${_WITH_STRSVIS} == 0
.PATH: ${.CURDIR}/../../contrib/libc-vis
SRCS+=		vis.c
CFLAGS+=	-I${.CURDIR}/../../contrib/libc-vis \
		-I${.CURDIR}/../../lib/libc/include
.endif

_WITH_REALLOCARRAY!= grep -c reallocarray /usr/include/stdlib.h || true
.if ${_WITH_REALLOCARRAY} == 0
.PATH: ${.CURDIR}/../../lib/libc/stdlib
INCS+=		stdlib.h
SRCS+=		reallocarray.c
CFLAGS+=	-I${.CURDIR}/../../lib/libc/include
.endif

_WITH_UTIMENS!= grep -c utimensat /usr/include/sys/stat.h || true
.if ${_WITH_UTIMENS} == 0
SYSINCS+=	stat.h
SRCS+=		futimens.c utimensat.c
.endif

.if empty(SRCS)
SRCS=		dummy.c
.endif

.if defined(CROSS_BUILD_TESTING)
SUBDIR=		cross-build
.endif

.include <bsd.lib.mk>
