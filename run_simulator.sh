config=$1

main_memory_size=$(yq -er '.config.memory_size' "$config")
page_size=$(yq -er '.config.page_size' "$config")
cache_size=$(yq -er '.config.cache_size' "$config")
line_size=$(yq -er '.config.line_size' "$config")
assoc=$(yq -er '.config.assoc' "$config")
replacement_policy=$(yq -er '.config.replacement_policy' "$config")
write_policy=$(yq -er '.config.write_policy' "$config")
write_allocate=$(yq -er '.config.write_allocate' "$config")
trace_file=$(yq -er '.config.trace_file' "$config")
verbose=$(yq -r '.config.verbose' "$config")

cd ./build/

./Cache-Simulator --memory_size $main_memory_size --page_size $page_size --cache_size $cache_size --line_size $line_size --assoc $assoc --replacement_policy $replacement_policy --write_policy $write_policy --write_allocate $write_allocate --verbose $verbose --trace_file $trace_file
