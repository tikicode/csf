Contributions
--------------
MS1
Arijit worked on the main.cpp command line input logic. Arijit and Taran
worked on the cache initial implementation together.

MS2
Taran and Arijit worked together predomintantly and both commented and tested 
extensively.

MS3
Taran completed the final fifo cache. Arijit worked on the data analysis
to determine which cache configuration was most effective

Best Cache Analysis
--------------------
To determine the best cache parameters, we computed stats on the gcc.trace 
and swim.trace files given that those traces are from real programs.

Using a shell script (test_cache.sh), we determined cache statistics
for each trace file using the following combinations of inputs:

set_sizes="1 2 4 8 16 32 64 128 256"
block_sizes="1 2 4 8 16 32 64 128"
byte_sizes="4 8 16 32"
write_policy=("write-allocate" "no-write-allocate")
write_through=("write-back" "write-through")
cache_type=("lru" "fifo")

We found these parameters to be the most comprehensive since the L1 
cache of most modern CPUs is typically 64KB per core 
(https://www.quora.com/What-is-the-typical-size-of-cache-memory).
Our tests resulted in cache sizes from 8B to 128KB, which we found 
adequate for maximal coverage. We could've also tested cache sizes 
bigger than 128KB, but chose not to because we wanted to specifically 
analyze an L1 cache. Values much lower than 64KB represent cache sizes
for older generations of CPUs. We analyzed the following range of 
cache sizes:

test_cache_sizes = [1024, 4096, 16384, 65536, 262144]

Using a jupyter notebook (stats.ipynb), we created a dataframe containing
our test results for each trace file we tested. For fair performance 
evaluation, we first found the best cache of a given total cache size
then compared the best caches of each size. To determine the best 
cache for a given cache size, we computed the load hit/miss ratio
and store hit/miss ratio. We sorted the caches by these metrics and 
chose the cache with the highest ratio at each size to evaluate.
We also evaluated overall best performance by the caches with the
highest total hit to total miss ratio and best speed by the caches
with the lowest number of total cycles.

We chose these metrics for evaluation because we were most concerned
with cache speed and accuracy. Specifically for the hit/miss ratio,
we wanted to evaluate if different parameters for loading and storing
led to different performance in each for the same cache size. 

Results
-------
Based on the data we found, the best performing caches were predomintantly
LRU based caches. This was apparent in all of the cache sizes
we tested in both trace files. Logically, evicting the least 
recently used cache makes the most sense for our traces because
in programs, cache values accessed often will likely continue 
to be accessed throughout the runtime. While this is not always
the case, our testing shows it to be true for the traces we tested.

Write-allocate was the most commonly seen write policy in the best 
performing caches we found. However, we found that write-allocate
was seen most often for caches with larger capacity, particularly 
in the swim.trace tests. This could be because larger caches 
are able to store more values so the evicted value is the
the value least likely to be accessed again, especially since 
nearly all winning caches used LRU based caches. However, for 
load performance in smaller caches, no-write-allocate was seen
most often, potentially because smaller caches can only store
a limited number of cached values and allocating could eject 
often loaded values.

Many of the top scoring caches used both write-back and write-through.
Use write-back reduces the total cycle count compared to write-through.
All of the caches which used the fewest cyles used write-allocate and 
write-back. Additionally, smaller byte sizes were preferred because
many of our operations use for loops to traverse through bytes in 
a block. Smaller byte sizes for each block reduce the time required 
for linear search through blocks. However, this tends to reduce the
hit/miss ratio since larger block sizes can fit more adjacent values
in memory.

Lastly, for load hit performance, the general trend was
that larger set sizes and a higher number of bytes per 
block lead to the best performance while for storing values, 
a higher block size was preferred. For storing values, offering a 
larger locality with a higher number of blocks and a larger
block wise storage capacity. For loading values, a higher number
of sets allows for better indexing because fewer values are mapped
to each set, reducing the likelihood of collisions. Overall 
top performers struck a balance between the number of sets and blocks
or skewed towards fewer sets and more blocks in the case of gcc.trace.
However, in all cases, the highest byte size was preferred. Larger 
caches performed better with diminishing returns as the size of
the caches increased. 

Based on our testing, we determined that the best cache configuration
for a 64KB cache, which is the most common size for cpu based caches,
maintains the highest possible byte size, strikes a balance between
number of sets and number of blocks, skewing toward a higher number 
of blocks, uses LRU for its ejection method, and is write-back to
reduce the total number of cycles required. Our optimal parameters are:

<insert>