# $FreeBSD: releng/11.0/lib/libc/powerpc64/gen/Makefile.inc 277078 2015-01-12 18:13:38Z emaste $

SRCS += _ctx_start.S fabs.S flt_rounds.c fpgetmask.c fpgetround.c \
	fpgetsticky.c fpsetmask.c fpsetround.c \
	infinity.c ldexp.c makecontext.c _setjmp.S \
	setjmp.S sigsetjmp.S signalcontext.c syncicache.c \
	_set_tp.c \
	trivial-getcontextx.c


