#!/bin/sh
# $FreeBSD: releng/11.0/usr.bin/units/tests/basics_test.sh 269084 2014-07-25 01:29:22Z jmmv $

base=`basename $0`

echo "1..3"

assert_equals() {
    testnum="$1"
    expected="$2"
    fn="$3"
    if [ "$expected" = "$($fn)" ]
    then
        echo "ok $testnum - $fn"
    else
        echo "not ok $testnum - $fn"
    fi
}

assert_equals 1 1 "units -t ft ft"
assert_equals 2 12 "units -t ft in"
assert_equals 3 0.083333333 "units -t in ft"
