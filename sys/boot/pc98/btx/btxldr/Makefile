# $FreeBSD: releng/11.0/sys/boot/pc98/btx/btxldr/Makefile 297283 2016-03-26 03:46:12Z bdrewery $

PROG=	btxldr
INTERNALPROG=
MAN=
SRCS=	btxldr.S

CFLAGS+=-DLOADER_ADDRESS=${LOADER_ADDRESS}
CFLAGS+=-I${.CURDIR}/../../../i386/common

.if defined(BTXLDR_VERBOSE)
CFLAGS+=-DBTXLDR_VERBOSE
.endif

LDFLAGS=-e start -Ttext ${LOADER_ADDRESS} -Wl,-N,-S,--oformat,binary

.include <bsd.prog.mk>

# XXX: clang integrated-as doesn't grok .codeNN directives yet
CFLAGS.btxldr.S=	${CLANG_NO_IAS}
