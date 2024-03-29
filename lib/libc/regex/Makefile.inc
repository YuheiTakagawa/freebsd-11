#	from @(#)Makefile.inc	8.1 (Berkeley) 6/4/93
# $FreeBSD: releng/11.0/lib/libc/regex/Makefile.inc 262722 2014-03-04 02:19:39Z marcel $

# regex sources
.PATH: ${LIBC_SRCTOP}/regex

CFLAGS+=-DPOSIX_MISTAKE

SRCS+=	regcomp.c regerror.c regexec.c regfree.c

SYM_MAPS+=${LIBC_SRCTOP}/regex/Symbol.map

MAN+=	regex.3
MAN+=	re_format.7

MLINKS+=regex.3 regcomp.3 regex.3 regexec.3 regex.3 regerror.3
MLINKS+=regexec.3 regfree.3
