# $FreeBSD: releng/11.0/sys/modules/vnic/mrmlbus/Makefile 300294 2016-05-20 11:00:06Z wma $

SYSDIR?=${.CURDIR}/../../..
.include "${SYSDIR}/conf/kern.opts.mk"

S=	${.CURDIR}/../../..

.PATH: $S/dev/vnic

KMOD=	mrmlbus
SRCS=	device_if.h bus_if.h opt_platform.h pci_if.h ofw_bus_if.h miibus_if.h lmac_if.h
SRCS+=	mrml_bridge.c

CFLAGS+=	-DFDT

.include <bsd.kmod.mk>
