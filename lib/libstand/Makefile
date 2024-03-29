# $FreeBSD: releng/11.0/lib/libstand/Makefile 298107 2016-04-16 07:45:30Z gjb $
# Originally from	$NetBSD: Makefile,v 1.21 1997/10/26 22:08:38 lukem Exp $
#
# Notes:
# - We don't use the libc strerror/sys_errlist because the string table is
#   quite large.
#

PACKAGE=lib${LIB}
MK_PROFILE=	no
MK_SSP=		no

.include <src.opts.mk>

LIBSTAND_SRC?=	${.CURDIR}
LIBSTAND_CPUARCH?=${MACHINE_CPUARCH}
LIBC_SRC=	${LIBSTAND_SRC}/../libc

LIB=		stand
NO_PIC=
INCS?=		stand.h
MAN?=		libstand.3

WARNS?=		0

CFLAGS+= -I${LIBSTAND_SRC}

# standalone components and stuff we have modified locally
SRCS+=	gzguts.h zutil.h __main.c assert.c bcd.c environment.c getopt.c gets.c \
	globals.c pager.c printf.c strdup.c strerror.c strtol.c strtoul.c random.c \
	sbrk.c twiddle.c zalloc.c zalloc_malloc.c

# private (pruned) versions of libc string functions
SRCS+=	strcasecmp.c

.PATH: ${LIBC_SRC}/net

SRCS+= ntoh.c

# string functions from libc
.PATH: ${LIBC_SRC}/string
SRCS+=	bcmp.c bcopy.c bzero.c ffs.c fls.c \
	memccpy.c memchr.c memcmp.c memcpy.c memmove.c memset.c \
	qdivrem.c strcat.c strchr.c strcmp.c strcpy.c \
	strcspn.c strlcat.c strlcpy.c strlen.c strncat.c strncmp.c strncpy.c \
	strpbrk.c strrchr.c strsep.c strspn.c strstr.c strtok.c swab.c
.if ${MACHINE_CPUARCH} == "arm"
.PATH: ${LIBC_SRC}/arm/gen

# Do not generate movt/movw, because the relocation fixup for them does not
# translate to the -Bsymbolic -pie format required by self_reloc() in loader(8).
# Also, the fpu is not available in a standalone environment.
.if ${COMPILER_VERSION} < 30800
CFLAGS.clang+=	-mllvm -arm-use-movt=0
.else
CFLAGS.clang+=	-mno-movt
.endif
CFLAGS.clang+=	-mfpu=none

# Compiler support functions
.PATH: ${LIBSTAND_SRC}/../../contrib/compiler-rt/lib/builtins/
# __clzsi2 and ctzsi2 for various builtin functions
SRCS+=	clzsi2.c ctzsi2.c
# Divide and modulus functions called by the compiler
SRCS+=	 divmoddi4.c  divmodsi4.c  divdi3.c  divsi3.c  moddi3.c  modsi3.c
SRCS+=	udivmoddi4.c udivmodsi4.c udivdi3.c udivsi3.c umoddi3.c umodsi3.c

.PATH: ${LIBSTAND_SRC}/../../contrib/compiler-rt/lib/builtins/arm/
SRCS+=	aeabi_idivmod.S aeabi_ldivmod.S aeabi_uidivmod.S aeabi_uldivmod.S
SRCS+=	aeabi_memcmp.S aeabi_memcpy.S aeabi_memmove.S aeabi_memset.S
.endif

.if ${MACHINE_CPUARCH} == "aarch64" || ${MACHINE_CPUARCH} == "riscv"
.PATH: ${LIBC_SRC}/${MACHINE_CPUARCH}/gen
.endif

.if ${MACHINE_CPUARCH} == "powerpc"
.PATH: ${LIBC_SRC}/quad
SRCS+=	ashldi3.c ashrdi3.c
SRCS+=	syncicache.c
.endif

# uuid functions from libc
.PATH: ${LIBC_SRC}/uuid
SRCS+= uuid_create_nil.c uuid_equal.c uuid_from_string.c uuid_is_nil.c uuid_to_string.c

# _setjmp/_longjmp
.PATH: ${LIBSTAND_SRC}/${LIBSTAND_CPUARCH}
SRCS+=	_setjmp.S

# decompression functionality from libbz2
# NOTE: to actually test this functionality after libbz2 upgrade compile
# loader(8) with LOADER_BZIP2_SUPPORT defined
.PATH: ${LIBSTAND_SRC}/../../contrib/bzip2
CFLAGS+= -DBZ_NO_STDIO -DBZ_NO_COMPRESS
SRCS+=	libstand_bzlib_private.h

.for file in bzlib.c crctable.c decompress.c huffman.c randtable.c
SRCS+=	_${file}
CLEANFILES+=	_${file}

_${file}: ${file}
	sed "s|bzlib_private\.h|libstand_bzlib_private.h|" \
	    ${.ALLSRC} > ${.TARGET}
.endfor

CLEANFILES+= libstand_bzlib_private.h
libstand_bzlib_private.h: bzlib_private.h
	sed -e 's|<stdlib.h>|"stand.h"|' \
		${.ALLSRC} > ${.TARGET}

# decompression functionality from libz
.PATH: ${LIBSTAND_SRC}/../libz
CFLAGS+=-DHAVE_MEMCPY -I${LIBSTAND_SRC}/../libz
SRCS+=	adler32.c crc32.c libstand_zutil.h libstand_gzguts.h

.for file in infback.c inffast.c inflate.c inftrees.c zutil.c
SRCS+=	_${file}
CLEANFILES+=	_${file}

_${file}: ${file}
	sed -e "s|zutil\.h|libstand_zutil.h|" \
	    -e "s|gzguts\.h|libstand_gzguts.h|" \
	    ${.ALLSRC} > ${.TARGET}
.endfor

# depend on stand.h being able to be included multiple times
.for file in zutil.h gzguts.h
CLEANFILES+= libstand_${file}
libstand_${file}: ${file}
	sed -e 's|<fcntl.h>|"stand.h"|' \
	    -e 's|<stddef.h>|"stand.h"|' \
	    -e 's|<string.h>|"stand.h"|' \
	    -e 's|<stdio.h>|"stand.h"|' \
	    -e 's|<stdlib.h>|"stand.h"|' \
	    ${.ALLSRC} > ${.TARGET}
.endfor

# io routines
SRCS+=	closeall.c dev.c ioctl.c nullfs.c stat.c \
	fstat.c close.c lseek.c open.c read.c write.c readdir.c

# network routines
SRCS+=	arp.c ether.c inet_ntoa.c in_cksum.c net.c udp.c netif.c rpc.c

# network info services:
SRCS+=	bootp.c rarp.c bootparam.c

# boot filesystems
SRCS+=	ufs.c nfs.c cd9660.c tftp.c gzipfs.c bzipfs.c
SRCS+=	dosfs.c ext2fs.c
SRCS+=	splitfs.c
SRCS+=	pkgfs.c
.if ${MK_NAND} != "no"
SRCS+=	nandfs.c
.endif

.include <bsd.stand.mk>
.include <bsd.lib.mk>
