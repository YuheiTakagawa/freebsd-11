# $FreeBSD: releng/11.0/sys/modules/drm/via/Makefile 276606 2015-01-03 03:35:18Z imp $

.PATH:	${.CURDIR}/../../../dev/drm
KMOD	= via
SRCS	= via_dma.c via_dmablit.c via_drv.c via_irq.c via_map.c via_mm.c \
	via_verifier.c via_video.c
SRCS	+= device_if.h bus_if.h pci_if.h opt_drm.h

.if defined(DRM_DEBUG)
DRM_DEBUG_OPT= "\#define DRM_DEBUG 1"
.endif

.if !defined(DRM_NOLINUX) && \
     (${MACHINE_CPUARCH} == "i386" || ${MACHINE_CPUARCH} == "amd64")
DRM_LINUX_OPT= "\#define DRM_LINUX 1"
.endif

opt_drm.h:
	touch opt_drm.h
	echo $(DRM_DEBUG_OPT) >> opt_drm.h
	echo $(DRM_LINUX_OPT) >> opt_drm.h

.include <bsd.kmod.mk>

CWARNFLAGS.via_dma.c=		-Wno-unused-value
CWARNFLAGS.via_dmablit.c=	-Wno-unused-value
