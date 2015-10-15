#!/bin/bash
#
# This runs a series of tests to test cachelocking performance. It takes as
# a parameter the total number of cacheways on the system. If the parameter
# is not provided, then it defaults to 8 ways.
#
# The test accesses elements in an array by striding across it, looping
# to do these accesses repeatedly. As the amount of memory cachelocked is
# increased, the performance should decrease (since less cache is available
# to be used by the test program). With all cache ways locked, the cache
# should be being by-passed completely.
#
# This test starts by locking the specified number of ways when running the
# test. After each test run, it decreases the number of locked ways by one,
# running the test again. This ensures that data doesn't get loaded into the
# cache on a run, then locked into place and used in future test runs.
#

if [ -z $1 ]
then
	NUM_WAYS=8
else
	NUM_WAYS=$1
fi

i=$NUM_WAYS

while [ $NUM_WAYS -ge 0 ]
do
	echo "******"
	echo "Now running for nways = $i"
	echo "******"

	sudo modprobe cachelock nways=$i memsize=2097152 touch=0
	echo Running with $i way(s) locked...
	sudo ./cachetime -s2097152
	echo
	sudo modprobe -r cachelock

	i=`expr $i - 1`
done
