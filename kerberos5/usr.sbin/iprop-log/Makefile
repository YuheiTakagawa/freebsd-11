# $FreeBSD: releng/11.0/kerberos5/usr.sbin/iprop-log/Makefile 288200 2015-09-24 23:23:58Z bdrewery $

PROG=	iprop-log
MAN=	iprop-log.8
SRCS=	iprop-log.c iprop-commands.c iprop-commands.h kadm5_err.h
CFLAGS+=	-I${KRB5DIR}/lib/kadm5 \
		-I${KRB5DIR}/lib/krb5 \
		-I${KRB5DIR}/lib/roken \
		-I${KRB5DIR}/lib/sl \
		-I.
LIBADD=	kadm5srv hdb krb5 roken edit sl vers
LDFLAGS=${LDAPLDFLAGS}

CLEANFILES=	iprop-commands.h iprop-commands.c

iprop-commands.h: iprop-commands.in
	${SLC} ${.ALLSRC:M*.in}

iprop-commands.c: iprop-commands.h

.include <bsd.prog.mk>

.PATH: ${KRB5DIR}/lib/kadm5
