# !/usr/bin/env bash
# This bash script runs all the test cases present inside tests/ directory.

# Get the directory where this script resides
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "Running all test cases inside tests/"

for test in test-*;
do
	touch "$test/output.txt"

	for config in $test/*.yaml;
	do
		../run_simulator.sh $config > $test/output.txt
		cd "$SCRIPT_DIR"

		if grep -xFq "The test case with the given trace passed successfully!" "$test/output.txt"; then
			echo "$test: PASSED"
		else
			echo "$test: FAILED"
		fi 
	done

	rm "$test/output.txt"
done
