#! /bin/sh
# $FreeBSD: releng/11.0/usr.bin/bmake/tests/suffixes/src_wild2/legacy_test.sh 263346 2014-03-19 12:29:20Z jmmv $

. $(dirname $0)/../../common.sh

# Description
DESC="Source wildcard expansion (2)."

# Setup
TEST_COPY_FILES="TEST1.a 644	TEST2.a 644"

# Reset
TEST_CLEAN="TEST1.b"

# Run
TEST_N=1
TEST_1="-r test1"

eval_cmd $*
