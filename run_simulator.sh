config=$1

memory_size=$(yq -r '.memory.memory_size' "$config")
page_size=$(yq -r '.memory.page_size' "$config")
trace_file=$(yq -r '.simulation.trace_file' "$config")
verbose=$(yq -r '.simulation.verbose' "$config")

num_levels=$(yq -r '.cache_hierarchy.levels | length' "$config")

cmd=(
  ./Cache-Simulator
  --memory_size "$memory_size"
  --page_size "$page_size"
  --num_cache_levels "$num_levels"
  --trace_file "$trace_file"
  --verbose "$verbose"
)

for ((i = 0; i < num_levels; i++)); 
do
  name=$(yq -r ".cache_hierarchy.levels[$i].name" "$config")
  cache_size=$(yq -r ".cache_hierarchy.levels[$i].cache_size" "$config")
  line_size=$(yq -r ".cache_hierarchy.levels[$i].line_size" "$config")
  assoc=$(yq -r ".cache_hierarchy.levels[$i].assoc" "$config")
  replacement_policy=$(yq -r ".cache_hierarchy.levels[$i].replacement_policy" "$config")
  write_policy=$(yq -r ".cache_hierarchy.levels[$i].write_policy" "$config")
  write_allocate=$(yq -r ".cache_hierarchy.levels[$i].write_allocate" "$config")

  level_config="${name},${cache_size},${line_size},${assoc},${replacement_policy},${write_policy},${write_allocate}"

  cmd+=(--cache_level "$level_config")
done

cd ./build/

echo "Running: ${cmd[@]}"

"${cmd[@]}"
