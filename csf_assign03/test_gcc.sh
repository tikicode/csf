#!/bin/bash

# Specify the list of cache sizes and byte_sizes you want to test
set_sizes="1 2 4 8 16 32 64 128 256"
block_sizes="1 2 4 8 16 32 64 128 256"
byte_sizes="4 8 16 32 64 128 256"

# Specify the list of options to test
write_policy=("write-allocate" "no-write-allocate")
write_through=("write-back" "write-through")
cache_type=("lru" "fifo")

# Loop through the combinations for the gcc_trace
for s_size in $set_sizes; do
    for b_size in $block_sizes; do
        for byte_s in $byte_sizes; do
            for wp in "${write_policy[@]}"; do
                for wt in "${write_through[@]}"; do
                    for cache in "${cache_type[@]}"; do
                            output_file="${s_size}_${b_size}_${byte_s}_${wp}_${wt}_${cache}.txt"
                            echo "Running ./csim $s_size $b_size $byte_s $wp $wt $cache < traces/gcc.trace > gcc_output/$output_file"
                            ./csim $s_size $b_size $byte_s $wp $wt $cache < traces/gcc.trace > gcc_output/$output_file
                            echo "Done. Output saved to $output_file"
                    done
                done
            done
        done
    done
done