# $FreeBSD: releng/11.0/lib/libnv/tests/Makefile 299094 2016-05-04 23:20:53Z ngie $

ATF_TESTS_CXX=	\
	dnv_tests \
	nv_array_tests \
	nv_tests \

TAP_TESTS_C+=	nvlist_add_test
TAP_TESTS_C+=	nvlist_exists_test
TAP_TESTS_C+=	nvlist_free_test
TAP_TESTS_C+=	nvlist_get_test
TAP_TESTS_C+=	nvlist_move_test
TAP_TESTS_C+=	nvlist_send_recv_test

LIBADD+=	nv

WARNS?=		3

.include <bsd.test.mk>
