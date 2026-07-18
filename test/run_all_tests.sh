#!/usr/bin/env bash
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

		expected_total_latency=$(yq -r ".expected.total_latency" "$config")
		actual_total_latency=$(awk '/Total Latency/ {print $NF}' "$test/output.txt")

		if [[ "$expected_total_latency" == "$actual_total_latency" ]]; then
			echo "$test: PASSED"
			echo "Expected: $expected_total_latency"
			echo "Actual:   $actual_total_latency"
		else
			echo "$test: FAILED"
			echo "Expected: $expected_total_latency"
			echo "Actual:   $actual_total_latency"
		fi
	done

	rm "$test/output.txt"
done
