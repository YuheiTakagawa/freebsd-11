# $FreeBSD: releng/11.0/sys/boot/fdt/Makefile 298821 2016-04-29 22:42:59Z gonzo $

.PATH:		${.CURDIR}/../../contrib/libfdt/

LIB=		fdt
INTERNALLIB=

# Vendor sources of libfdt.
SRCS+=		fdt.c fdt_ro.c fdt_wip.c fdt_sw.c fdt_rw.c fdt_strerror.c \
		fdt_empty_tree.c

# Loader's fdt commands extension sources.
SRCS+=		fdt_loader_cmd.c fdt_overlay.c

CFLAGS+=	-I${.CURDIR}/../../contrib/libfdt/ -I${.CURDIR}/../common/

CFLAGS+=	-ffreestanding

.if ${MACHINE_CPUARCH} == "powerpc" || ${MACHINE_CPUARCH} == "arm" || ${MACHINE_CPUARCH} == "mips"
CFLAGS+=	-msoft-float
.endif

.if ${MACHINE_ARCH} == "powerpc64"
CFLAGS+=	-m32
.endif

CFLAGS+=	-Wformat -Wall

.include <bsd.stand.mk>
.include <bsd.lib.mk>
