# $FreeBSD: releng/11.0/usr.sbin/gssd/Makefile 278691 2015-02-13 18:34:04Z markj $

.include <src.opts.mk>

PROG=	gssd
MAN=	gssd.8
SRCS=	gssd.c gssd.h gssd_svc.c gssd_xdr.c gssd_prot.c

CFLAGS+= -I.
WARNS?= 1

LIBADD=	gssapi
.if ${MK_KERBEROS_SUPPORT} != "no"
LIBADD+=	krb5 roken
.else
CFLAGS+= -DWITHOUT_KERBEROS
.endif

CLEANFILES= gssd_svc.c gssd_xdr.c gssd.h

RPCSRC=	${.CURDIR}/../../sys/kgssapi/gssd.x
RPCGEN= RPCGEN_CPP=${CPP:Q} rpcgen -L -C -M

gssd_svc.c: ${RPCSRC} gssd.h
	${RPCGEN} -m -o ${.TARGET} ${RPCSRC}

gssd_xdr.c: ${RPCSRC} gssd.h
	${RPCGEN} -c -o ${.TARGET} ${RPCSRC}

gssd.h: ${RPCSRC}
	${RPCGEN} -h -o ${.TARGET} ${RPCSRC}

.PATH:	${.CURDIR}/../../sys/kgssapi

.include <bsd.prog.mk>
