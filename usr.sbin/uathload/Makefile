# $FreeBSD: releng/11.0/usr.sbin/uathload/Makefile 296889 2016-03-15 04:42:37Z imp $

PROG=	uathload
MAN=	uathload.8

SRCS=	uathload.c
FILES=	ar5523.bin
FILESDIR=	${SHAREDIR}/firmware
FILESMODE=	444

CLEANFILES=	ar5523.bin

ar5523.bin: ${.CURDIR}/../../sys/contrib/dev/uath/ar5523.bin.uu
	uudecode -p ${.CURDIR}/../../sys/contrib/dev/uath/ar5523.bin.uu > ${.TARGET}

.include <bsd.prog.mk>
