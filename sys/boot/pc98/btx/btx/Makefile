# $FreeBSD: releng/11.0/sys/boot/pc98/btx/btx/Makefile 297283 2016-03-26 03:46:12Z bdrewery $

PROG=	btx
INTERNALPROG=
MAN=
SRCS=	btx.S

.if defined(BOOT_BTX_NOHANG)
BOOT_BTX_FLAGS=0x1
.else
BOOT_BTX_FLAGS=0x0
.endif

CFLAGS+=-DBTX_FLAGS=${BOOT_BTX_FLAGS}
CFLAGS+=-I${.CURDIR}/../../../i386/common

.if defined(BTX_SERIAL)
BOOT_COMCONSOLE_PORT?= 0x238
BOOT_COMCONSOLE_SPEED?= 9600
B2SIOFMT?=	0x3

CFLAGS+=-DBTX_SERIAL -DSIOPRT=${BOOT_COMCONSOLE_PORT} \
	-DSIOFMT=${B2SIOFMT} -DSIOSPD=${BOOT_COMCONSOLE_SPEED}
.endif

ORG=	0x9000

LDFLAGS=-e start -Ttext ${ORG} -Wl,-N,-S,--oformat,binary

.include <bsd.prog.mk>

# XXX: clang integrated-as doesn't grok .codeNN directives yet
CFLAGS.btx.S=		${CLANG_NO_IAS}
