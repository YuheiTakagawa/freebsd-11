# $FreeBSD: releng/11.0/lib/libclang_rt/Makefile.inc 296417 2016-03-05 20:24:31Z dim $

.include <src.opts.mk>

CRTARCH=${MACHINE_CPUARCH:C/amd64/x86_64/}
CRTSRC=${.CURDIR}/../../../contrib/compiler-rt

CLANGDIR=/usr/lib/clang/3.8.0
LIBDIR=${CLANGDIR}/lib/freebsd

NO_PIC=
MK_PROFILE=no

WARNS?=0

SSP_CFLAGS=
CFLAGS+=-DNDEBUG
CFLAGS+=${PICFLAG}
CFLAGS+=-fno-builtin
CFLAGS+=-fno-exceptions
CFLAGS+=-fno-rtti
.if ${COMPILER_VERSION} >= 30700
CFLAGS+=-fno-sanitize=safe-stack
.endif
CFLAGS+=-fno-stack-protector
CFLAGS+=-funwind-tables
CFLAGS+=-fvisibility-inlines-hidden
CFLAGS+=-fvisibility=hidden
CFLAGS+=-I${CRTSRC}/lib
CXXFLAGS+=-std=c++11
