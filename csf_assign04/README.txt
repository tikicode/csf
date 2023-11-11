CONTRIBUTIONS

Brief summary of how each team member contributed to the project.

Taran worked on the merge_sort method mostly and Arijit worked on the main mostly.
Both Arijit and Taran debugged and commented the code as well as worked on the experimentation.

REPORT

Test run with threshold 2097152
real    0m0.393s
user    0m0.383s
sys     0m0.008s

Test run with threshold 1048576
real    0m0.229s
user    0m0.393s
sys     0m0.020s

Test run with threshold 524288
real    0m0.208s
user    0m0.401s
sys     0m0.033s

Test run with threshold 262144
real    0m0.200s
user    0m0.399s
sys     0m0.052s

Test run with threshold 131072
real    0m0.199s
user    0m0.418s
sys     0m0.052s

Test run with threshold 65536
real    0m0.172s
user    0m0.427s
sys     0m0.079s

Test run with threshold 32768
real    0m0.172s
user    0m0.441s
sys     0m0.085s

Test run with threshold 16384
real    0m0.241s
user    0m0.504s
sys     0m0.114s

Pt. 2
As the threshold is lowered, the execution time reduces. This happens because the data is divided into smaller 
segments, which are quicker to process and require fewer resources, resulting in a quicker execution time. Still,
there is a limit to how much execution time can decrease due to the overhead involved in the CPU's  scheduling of 
computations. This is why time reductions are more significant earlier on, but they start to level out/not exist as 
the threshold gets lower and lower.

Conversely, as the threshold is reduced even more, the user time sees an increase generally. This might be attributed to a higher 
amount of processing being undertaken by the CPU. The CPU also has a limited number of cores so as fork creates child processes, 
benefits are only seen until the number of processes exceeds the number of cores available to handle each process. Having a 
huge number of child processes can be detrimental because every new thread contains a duplicate of the parent process.
The system time, on the other hand, shows inconsistent variations, possibly due to fluctuations in disk I/O or the scheduling process. 

The distribution of workload across various processes and CPU cores is managed by the kernel and some processes may deal with more
complex calculations, while others tackle simpler tasks. The variations in execution time are influenced by the current workload 
of the CPU cores.