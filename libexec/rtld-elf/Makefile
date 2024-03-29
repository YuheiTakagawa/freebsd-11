# $FreeBSD: releng/11.0/libexec/rtld-elf/Makefile 302325 2016-07-03 17:28:39Z bdrewery $

# Use the following command to build local debug version of dynamic
# linker:
# make DEBUG_FLAGS=-g DEBUG=-DDEBUG MK_TESTS=no all

.include <src.opts.mk>
PACKAGE=	clibs
MK_SSP=		no

PROG?=		ld-elf.so.1
.if (${PROG:M*ld-elf32*} != "")
TAGS+=		lib32
.endif
SRCS=		rtld_start.S \
		reloc.c rtld.c rtld_lock.c rtld_printf.c map_object.c \
		malloc.c xmalloc.c debug.c libmap.c
MAN=		rtld.1
CSTD?=		gnu99
CFLAGS+=	-Wall -DFREEBSD_ELF -DIN_RTLD -ffreestanding
CFLAGS+=	-I${SRCTOP}/lib/csu/common
.if exists(${.CURDIR}/${MACHINE_ARCH})
RTLD_ARCH=	${MACHINE_ARCH}
.else
RTLD_ARCH=	${MACHINE_CPUARCH}
.endif
CFLAGS+=	-I${.CURDIR}/${RTLD_ARCH} -I${.CURDIR}
.if ${MACHINE_ARCH} == "powerpc64"
LDFLAGS+=	-nostdlib -e _rtld_start
.else
LDFLAGS+=	-nostdlib -e .rtld_start
.endif
WARNS?=		2
INSTALLFLAGS=	-C -b
PRECIOUSPROG=
BINDIR=		/libexec
SYMLINKS=	${BINDIR}/${PROG} ${LIBEXECDIR}/${PROG}
MLINKS=		rtld.1 ld-elf.so.1.1 \
		rtld.1 ld.so.1

.if ${MACHINE_CPUARCH} == "sparc64"
CFLAGS+=	-fPIC
.else
CFLAGS+=	-fpic
.endif
CFLAGS+=	-DPIC $(DEBUG)
.if ${MACHINE_CPUARCH} == "amd64" || ${MACHINE_CPUARCH} == "i386"
CFLAGS+=	-fvisibility=hidden
.endif
LDFLAGS+=	-shared -Wl,-Bsymbolic
LIBADD=		c_pic
.if ${MK_TOOLCHAIN} == "no"
LDFLAGS+=	-L${LIBCDIR}
.endif

.if ${MACHINE_CPUARCH} == "arm"
# Some of the required math functions (div & mod) are implemented in
# libcompiler_rt on ARM. The library also needs to be placed first to be
# correctly linked. As some of the functions are used before we have
# shared libraries.
LIBADD+=	compiler_rt
.endif



.if ${MK_SYMVER} == "yes"
VERSION_DEF=	${LIBCSRCDIR}/Versions.def
SYMBOL_MAPS=	${.CURDIR}/Symbol.map
VERSION_MAP=	Version.map
LDFLAGS+=	-Wl,--version-script=${VERSION_MAP}

.if exists(${.CURDIR}/${RTLD_ARCH}/Symbol.map)
SYMBOL_MAPS+=	${.CURDIR}/${RTLD_ARCH}/Symbol.map
.endif
.endif

.sinclude "${.CURDIR}/${RTLD_ARCH}/Makefile.inc"

# Since moving rtld-elf to /libexec, we need to create a symlink.
# Fixup the existing binary that's there so we can symlink over it.
beforeinstall:
.if exists(${DESTDIR}/usr/libexec/${PROG}) && ${MK_STAGING} == "no"
	-chflags -h noschg ${DESTDIR}/usr/libexec/${PROG}
.endif

.PATH: ${.CURDIR}/${RTLD_ARCH}

.if ${MK_TESTS} != "no"
SUBDIR+=	tests
.endif

.include <bsd.prog.mk>
${PROG_FULL}:	${VERSION_MAP}
.include <bsd.symver.mk>
