#$FreeBSD: releng/11.0/sys/modules/ixv/Makefile 294444 2016-01-20 16:45:39Z bdrewery $

.PATH:  ${.CURDIR}/../../dev/ixgbe

KMOD    = if_ixv
SRCS    = device_if.h bus_if.h pci_if.h pci_iov_if.h
SRCS	+= opt_inet.h opt_inet6.h opt_rss.h
SRCS    += if_ixv.c ix_txrx.c ixgbe_osdep.c
# Shared source
SRCS    += ixgbe_common.c ixgbe_api.c ixgbe_phy.c ixgbe_mbx.c ixgbe_vf.c
SRCS    += ixgbe_dcb.c ixgbe_dcb_82598.c ixgbe_dcb_82599.c
SRCS    += ixgbe_82598.c ixgbe_82599.c ixgbe_x540.c ixgbe_x550.c
CFLAGS+= -I${.CURDIR}/../../dev/ixgbe -DSMP

.include <bsd.kmod.mk>
