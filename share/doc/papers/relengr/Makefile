#	From: @(#)Makefile	1.6 (Berkeley) 6/8/93
# $FreeBSD: releng/11.0/share/doc/papers/relengr/Makefile 136910 2004-10-24 15:33:08Z ru $

VOLUME=		papers
DOC=		releng
SRCS=		stubs 0.t 1.t 2.t 3.t
EXTRA=		ref.bib
MACROS=		-ms
USE_REFER=
CLEANFILES=	stubs

stubs:
	@(echo .R1; echo database ${.CURDIR}/ref.bib; echo .R2) > ${.TARGET}

.include <bsd.doc.mk>
