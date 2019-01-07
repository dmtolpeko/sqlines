#!/bin/sh

# runtest: input, source, target, expected
runtest()
{
	in=$1
	source=$2
	target=$3
	exp=$4

	#echo sqlines -s=${source} -t=${target} -in=${in}
	sqlines -s=${source} -t=${target} -in=${in} > /dev/null
	out=`echo "$in" | sed 's/\.sql$/_out.sql/'`
	if diff -rU2 $exp $out; then
		echo "PASS: ${in} -> ${target}"
	else
		echo "FAIL: ${in} -> ${target}"
	fi
}

runtest_all_targets()
{
	test=$1
	testprefix=`echo "$1" | sed 's/\.sql$//'`
	source=`echo "$1" | sed 's|\(.*\)/.*$|\1|'` # last directory

	#echo "XX: test:$test source:$source testprefix:$testprefix"
	for exp in ${testprefix}_expected_*.sql; do
		#echo "XX: exp:$exp"
		target=`echo "$exp" | sed 's|^.*_expected_\([^\.]*\)\.sql$|\1|'`
		#echo "XX: target:[$target]"
		if test "$target" = '*'; then
			echo "SKIP: no targets for test $test" >&2
			continue # no expedcted files
		fi
		runtest $test $source $target $exp
	done
}

for test in $@; do
	runtest_all_targets $test
done
