#	@(#)Makefile	8.2 (Berkeley) 4/4/94
# $FreeBSD: releng/11.0/libexec/ftpd/Makefile 301241 2016-06-03 06:24:03Z lidl $

.include <src.opts.mk>

PROG=	ftpd
MAN=	ftpd.8 ftpchroot.5
SRCS=	ftpd.c ftpcmd.y logwtmp.c popen.c

CFLAGS+=-DSETPROCTITLE -DLOGIN_CAP -DVIRTUAL_HOSTING
CFLAGS+=-I${.CURDIR}
YFLAGS=
WARNS?=	2
WFORMAT=0

LIBADD=	crypt xo util

# XXX Kluge! Conversation mechanism needs to be fixed.
LIBADD+=	opie md

LSDIR=	../../bin/ls
.PATH:	${.CURDIR}/${LSDIR}
SRCS+=	ls.c cmp.c print.c util.c
CFLAGS+=-Dmain=ls_main -I${.CURDIR}/${LSDIR}
LIBADD+=	m

.if ${MK_BLACKLIST_SUPPORT} != "no"
CFLAGS+= -DUSE_BLACKLIST -I${SRCTOP}/contrib/blacklist/include
SRCS+= blacklist.c
LIBADD+= blacklist
LDFLAGS+=-L${LIBBLACKLISTDIR}
.endif

.if ${MK_INET6_SUPPORT} != "no"
CFLAGS+=-DINET6
.endif

.if ${MK_PAM_SUPPORT} != "no"
CFLAGS+=-DUSE_PAM
LIBADD+=	pam
.endif

.include <bsd.prog.mk>
