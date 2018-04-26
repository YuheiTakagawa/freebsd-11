# $FreeBSD: releng/11.0/lib/libc/aarch64/Makefile.inc 281197 2015-04-07 09:52:14Z andrew $
#
# Machine dependent definitions for the arm 64-bit architecture.
#

# Long double is quad precision
GDTOASRCS+=strtorQ.c
MDSRCS+=machdep_ldisQ.c
SYM_MAPS+=${LIBC_SRCTOP}/aarch64/Symbol.map