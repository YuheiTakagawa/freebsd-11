# $FreeBSD: releng/11.0/sys/modules/cxgbe/iw_cxgbe/Makefile 290135 2015-10-29 08:28:39Z hselasky $

CXGBE=	${.CURDIR}/../../../dev/cxgbe
.PATH: ${CXGBE}/iw_cxgbe

KMOD=	iw_cxgbe
SRCS=	bus_if.h
SRCS+=	cm.c
SRCS+=	cq.c
SRCS+=	device.c
SRCS+=	device_if.h
SRCS+=	ev.c
SRCS+=	id_table.c
SRCS+=	mem.c
SRCS+=	opt_inet.h
SRCS+=	opt_ktr.h
SRCS+=	opt_ofed.h
SRCS+=	opt_sched.h
SRCS+=	pci_if.h
SRCS+=	pcib_if.h
SRCS+=	provider.c
SRCS+=	qp.c
SRCS+=	resource.c
SRCS+=	vnode_if.h

CFLAGS+= -I${CXGBE} -I${.CURDIR}/../../../ofed/include -DLINUX_TYPES_DEFINED
CFLAGS+= -I${.CURDIR}/../../../compat/linuxkpi/common/include

.include <bsd.kmod.mk>
