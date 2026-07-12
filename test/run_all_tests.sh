# !/usr/bin/env bash
# This bash script runs all the test cases present inside tests/ directory.

echo "Running all test cases inside tests/"

for test in test-*;
do
	touch "$test/output.txt"

	for config in $test/*.yaml;
	do
		test_name=$(yq -er '.name' "$config")
		echo "--------------------------------"
		echo "Running $test_name..."

		main_memory_size=$(yq -er '.config.memory_size' "$config")
		page_size=$(yq -er '.config.page_size' "$config")
		cache_size=$(yq -er '.config.cache_size' "$config")
		line_size=$(yq -er '.config.line_size' "$config")
		assoc=$(yq -er '.config.assoc' "$config")
		replacement_policy=$(yq -er '.config.replacement_policy' "$config")
		write_policy=$(yq -er '.config.write_policy' "$config")
		write_allocate=$(yq -r '.config.write_allocate' "$config")
		trace_file=$(yq -er '.config.trace_file' "$config")
		verbose=$(yq -r '.config.verbose' "$config")

		../build/Cache-Simulator --memory_size $main_memory_size --page_size $page_size --cache_size $cache_size --line_size $line_size --assoc $assoc --replacement_policy $replacement_policy --write_policy $write_policy --write_allocate $write_allocate --verbose $verbose --trace_file $trace_file > $test/output.txt

		if grep -xFq "The test case with the given trace passed successfully!" "$test/output.txt"; then
			echo "$test_name: PASSED"
		else
			echo "$test_name: FAILED"
		fi 
		# echo "--------------------------------"
	done

	rm "$test/output.txt"
done
