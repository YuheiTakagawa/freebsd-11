# $FreeBSD: releng/11.0/usr.bin/truss/Makefile 301715 2016-06-09 02:21:07Z jhb $

NO_WERROR=
PROG=	truss
SRCS=	main.c setup.c syscalls.c

LIBADD=	sysdecode

CFLAGS+= -I${.CURDIR} -I. -I${.CURDIR}/../../sys

ABIS+=		freebsd
# Each ABI is expected to have an ABI.c, MACHINE_ARCH-ABI.c or
# MACHINE_CPUARCH-ABI.c file that will be used to map the syscall arguments.
.if ${MACHINE_ARCH} == "aarch64"
ABIS+=		cloudabi64
.endif
.if ${MACHINE_CPUARCH} == "i386"
ABIS+=		i386-linux
.endif
.if ${MACHINE_CPUARCH} == "amd64"
ABIS+=		amd64-linux
ABIS+=		amd64-linux32
ABIS+=		freebsd32
ABIS+=		cloudabi64
.endif
.if ${MACHINE_ARCH} == "powerpc64"
ABIS+=		freebsd32
.endif

.for abi in ${ABIS}
# Find the right file to handle this ABI.
abi_src=
ABI_SRCS=	${abi}.c ${MACHINE_ARCH}-${abi}.c ${MACHINE_CPUARCH}-${abi}.c
.for f in ${ABI_SRCS}
.if exists(${.CURDIR}/${f}) && empty(abi_src)
abi_src=	${f}
.endif
.endfor
SRCS:=		${SRCS} ${abi_src}
.endfor

.include <bsd.prog.mk>
