# $FreeBSD: releng/11.0/tools/tools/iwn/iwnstats/Makefile 287314 2015-08-30 21:55:01Z adrian $

MAN=

.include <bsd.own.mk>

.PATH:	${.CURDIR}/../../../../sys/dev/iwn/

CFLAGS+=-I${.CURDIR}/../../../../sys/dev/iwn/
CFLAGS+=-I${.CURDIR}/../../../../sys/

PROG=	iwnstats

# Because of a clang preprocessor parser limitation causing this
# to not compile, use gcc for now.
#CC=	gcc

SRCS=	main.c iwn_ioctl.c

# CFLAGS.clang+= -fbracket-depth=512

.include <bsd.prog.mk>
