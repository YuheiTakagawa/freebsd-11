# $FreeBSD: releng/11.0/usr.sbin/ctm/mkCTM/Makefile 275054 2014-11-25 16:57:27Z bapt $

PROG=	mkctm
MAN=

LIBADD=	md

test:	mkctm
	rm -f tst.out*
	time ./mkctm -v -v /3c/210src  /a/r1/usr/src \
		2>a | md5 -p > /a/tst.out 
	ls -l /a/tst.out
	gzip -9 -v /a/tst.out
	ls -l /a/tst.out.gz
	# cd /usr/src/release && ctm -c -v -v  ${.CURDIR}/tst.out

test1:	mkctm
	rm -f tst.out*
	time ./mkctm -v -v /3c/210src  /home/ncvs/src \
		2> b | md5 -p > /a/tst2.out 
	ls -l /a/tst2.out
	gzip -9 -v /a/tst2.out
	ls -l /a/tst2.out.gz

.include <bsd.prog.mk>
