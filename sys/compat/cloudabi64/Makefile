# $FreeBSD: releng/11.0/sys/compat/cloudabi64/Makefile 297247 2016-03-24 21:47:15Z ed $

all:
	@echo "make sysent only"

sysent: cloudabi64_sysent.c cloudabi64_syscall.h cloudabi64_proto.h \
    cloudabi64_syscalls.c cloudabi64_systrace_args.c

cloudabi64_sysent.c cloudabi64_syscall.h cloudabi64_proto.h \
    cloudabi64_syscalls.c cloudabi64_systrace_args.c: \
    ../../kern/makesyscalls.sh ../../contrib/cloudabi/syscalls.master \
    syscalls.conf
	sh ../../kern/makesyscalls.sh ../../contrib/cloudabi/syscalls.master \
	    syscalls.conf
