# $FreeBSD: releng/11.0/bin/freebsd-version/Makefile 298107 2016-04-16 07:45:30Z gjb $

PACKAGE=runtime
SCRIPTS = freebsd-version
MAN = freebsd-version.1
CLEANFILES = freebsd-version freebsd-version.sh
NEWVERS = ${.CURDIR}/../../sys/conf/newvers.sh

freebsd-version.sh: ${.CURDIR}/freebsd-version.sh.in ${NEWVERS}
	eval $$(egrep '^(TYPE|REVISION|BRANCH)=' ${NEWVERS}) ; \
	if ! sed -e "\
			s/@@TYPE@@/$${TYPE}/g; \
			s/@@REVISION@@/$${REVISION}/g; \
			s/@@BRANCH@@/$${BRANCH}/g; \
		" ${.CURDIR}/freebsd-version.sh.in >${.TARGET} ; then \
		rm -f ${.TARGET} ; \
		exit 1 ; \
	fi

.include <bsd.prog.mk>
