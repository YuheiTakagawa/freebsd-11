#
# $FreeBSD: releng/11.0/sys/modules/cxgbe/tom/Makefile 269540 2014-08-04 22:37:02Z imp $
#

CXGBE=	${.CURDIR}/../../../dev/cxgbe
.PATH: ${CXGBE}/tom

KMOD=	t4_tom
SRCS=	bus_if.h
SRCS+=	device_if.h
SRCS+=	opt_inet.h
SRCS+=	opt_inet6.h
SRCS+=	pci_if.h
SRCS+=	t4_connect.c
SRCS+=	t4_cpl_io.c
SRCS+=	t4_ddp.c
SRCS+=	t4_listen.c
SRCS+=	t4_tom.c
SRCS+=	t4_tom_l2t.c

CFLAGS+= -I${CXGBE}

.include <bsd.kmod.mk>
