# $FreeBSD: releng/11.0/lib/libc/tests/Makefile 299704 2016-05-14 00:35:35Z vangyzen $

.include <src.opts.mk>

SUBDIR=		tls_dso

TESTS_SUBDIRS=	c063
TESTS_SUBDIRS+=	db
TESTS_SUBDIRS+=	gen
TESTS_SUBDIRS+=	hash
TESTS_SUBDIRS+=	iconv
TESTS_SUBDIRS+=	inet
TESTS_SUBDIRS+=	net
TESTS_SUBDIRS+=	nss
TESTS_SUBDIRS+=	regex
TESTS_SUBDIRS+=	resolv
TESTS_SUBDIRS+=	rpc
TESTS_SUBDIRS+=	setjmp
TESTS_SUBDIRS+=	stdio
TESTS_SUBDIRS+=	stdlib
TESTS_SUBDIRS+=	string
TESTS_SUBDIRS+=	sys
TESTS_SUBDIRS+=	termios
TESTS_SUBDIRS+=	tls
TESTS_SUBDIRS+=	ttyio

SUBDIR_DEPEND_tls= tls_dso

.if ${MK_LOCALES} != "no"
TESTS_SUBDIRS+=	locale
.endif

.if ${MK_SSP} != "no"
TESTS_SUBDIRS+=	ssp
.endif

.include <bsd.test.mk>
