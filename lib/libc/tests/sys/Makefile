# $FreeBSD: releng/11.0/lib/libc/tests/sys/Makefile 299094 2016-05-04 23:20:53Z ngie $

.include <bsd.own.mk>

ATF_TESTS_C+=			queue_test

# TODO: clone, lwp_create, lwp_ctl, posix_fadvise, recvmmsg,
# swapcontext
NETBSD_ATF_TESTS_C+=		access_test
NETBSD_ATF_TESTS_C+=		chroot_test
NETBSD_ATF_TESTS_C+=		clock_gettime_test
NETBSD_ATF_TESTS_C+=		connect_test
NETBSD_ATF_TESTS_C+=		dup_test
NETBSD_ATF_TESTS_C+=		fsync_test
NETBSD_ATF_TESTS_C+=		getcontext_test
NETBSD_ATF_TESTS_C+=		getgroups_test
NETBSD_ATF_TESTS_C+=		getitimer_test
NETBSD_ATF_TESTS_C+=		getlogin_test
NETBSD_ATF_TESTS_C+=		getpid_test
NETBSD_ATF_TESTS_C+=		getrusage_test
NETBSD_ATF_TESTS_C+=		getsid_test
NETBSD_ATF_TESTS_C+=		gettimeofday_test
NETBSD_ATF_TESTS_C+=		issetugid_test
NETBSD_ATF_TESTS_C+=		kevent_test
NETBSD_ATF_TESTS_C+=		kill_test
NETBSD_ATF_TESTS_C+=		link_test
NETBSD_ATF_TESTS_C+=		listen_test
NETBSD_ATF_TESTS_C+=		mincore_test
NETBSD_ATF_TESTS_C+=		mkdir_test
NETBSD_ATF_TESTS_C+=		mkfifo_test
NETBSD_ATF_TESTS_C+=		mknod_test
NETBSD_ATF_TESTS_C+=		mlock_test
NETBSD_ATF_TESTS_C+=		mmap_test
NETBSD_ATF_TESTS_C+=		mprotect_test
NETBSD_ATF_TESTS_C+=		msgctl_test
NETBSD_ATF_TESTS_C+=		msgget_test
NETBSD_ATF_TESTS_C+=		msgrcv_test
NETBSD_ATF_TESTS_C+=		msgsnd_test
NETBSD_ATF_TESTS_C+=		msync_test
NETBSD_ATF_TESTS_C+=		nanosleep_test
NETBSD_ATF_TESTS_C+=		pipe_test
NETBSD_ATF_TESTS_C+=		pipe2_test
NETBSD_ATF_TESTS_C+=		poll_test
NETBSD_ATF_TESTS_C+=		revoke_test
NETBSD_ATF_TESTS_C+=		select_test
NETBSD_ATF_TESTS_C+=		setrlimit_test
NETBSD_ATF_TESTS_C+=		setuid_test
NETBSD_ATF_TESTS_C+=		sigaction_test
NETBSD_ATF_TESTS_C+=		sigqueue_test
NETBSD_ATF_TESTS_C+=		sigtimedwait_test
NETBSD_ATF_TESTS_C+=		socketpair_test
NETBSD_ATF_TESTS_C+=		stat_test
NETBSD_ATF_TESTS_C+=		timer_create_test
NETBSD_ATF_TESTS_C+=		truncate_test
NETBSD_ATF_TESTS_C+=		ucontext_test
NETBSD_ATF_TESTS_C+=		umask_test
NETBSD_ATF_TESTS_C+=		unlink_test
NETBSD_ATF_TESTS_C+=		write_test

LIBADD.getpid_test+=		pthread
LIBADD.timer_create_test+=	rt

.include "../Makefile.netbsd-tests"

.if ${COMPILER_TYPE} == "gcc"
WARNS?=			3
.else
WARNS?=			4
.endif

FILESGROUPS+=		truncate_test_FILES

truncate_test_FILES=	truncate_test.root_owned
truncate_test_FILESDIR=	${TESTSDIR}
truncate_test_FILESMODE= 0600
truncate_test_FILESOWNER= root
truncate_test_FILESGRP= wheel
truncate_test_FILESPACKAGE=	${PACKAGE}

CLEANFILES=	truncate_test.root_owned
truncate_test.root_owned:
	dd if=/dev/null bs=1 count=1 of=${.TARGET} status=none

.include <bsd.test.mk>
