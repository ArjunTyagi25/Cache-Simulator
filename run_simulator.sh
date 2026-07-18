#!/usr/bin/env bash

# Get the directory where this script resides
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

config=$1

trace_file=$(yq -r '.simulation.trace_file' "$config")
verbose=$(yq -r '.simulation.verbose' "$config")

num_memory_levels=$(yq -r '.memory_hierarchy.levels | length' "$config")
num_cache_levels=$(yq -r '.cache_hierarchy.levels | length' "$config")

cmd=(
  ./Cache-Simulator
  --num_memory_levels "$num_memory_levels"
  --num_cache_levels "$num_cache_levels"
  --trace_file "$trace_file"
  --verbose "$verbose"
)

for ((i = 0; i < num_memory_levels; i++)); 
do
  name=$(yq -r ".memory_hierarchy.levels[$i].name" "$config")
  memory_size=$(yq -r ".memory_hierarchy.levels[$i].memory_size" "$config")
  page_size=$(yq -r ".memory_hierarchy.levels[$i].page_size" "$config")
  line_size=$(yq -r ".memory_hierarchy.levels[$i].line_size" "$config")
  read_latency=$(yq -r ".memory_hierarchy.levels[$i].read_latency" "$config")
  write_latency=$(yq -r ".memory_hierarchy.levels[$i].write_latency" "$config")

  level_config="${name},${memory_size},${page_size},${line_size},${read_latency},${write_latency}"

  cmd+=(--memory_level "$level_config")
done

for ((i = 0; i < num_cache_levels; i++)); 
do
  name=$(yq -r ".cache_hierarchy.levels[$i].name" "$config")
  cache_size=$(yq -r ".cache_hierarchy.levels[$i].cache_size" "$config")
  line_size=$(yq -r ".cache_hierarchy.levels[$i].line_size" "$config")
  assoc=$(yq -r ".cache_hierarchy.levels[$i].assoc" "$config")
  replacement_policy=$(yq -r ".cache_hierarchy.levels[$i].replacement_policy" "$config")
  write_policy=$(yq -r ".cache_hierarchy.levels[$i].write_policy" "$config")
  write_allocate=$(yq -r ".cache_hierarchy.levels[$i].write_allocate" "$config")
  read_latency=$(yq -r ".cache_hierarchy.levels[$i].read_latency" "$config")
  write_latency=$(yq -r ".cache_hierarchy.levels[$i].write_latency" "$config")

  level_config="${name},${cache_size},${line_size},${assoc},${replacement_policy},${write_policy},${write_allocate},${read_latency},${write_latency}"

  cmd+=(--cache_level "$level_config")
done

cd "$SCRIPT_DIR/build/"

echo "Running: ${cmd[@]}"

"${cmd[@]}"
