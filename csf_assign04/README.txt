CONTRIBUTIONS

Kedar worked on the implementation for merge sort, regarding the recursive calls, and maintained proper indexing.
Dayann worked on the forking, some of the error handling, and the merge function. 

REPORT
Test run with threshold 2097152

real	0m0.370s
user	0m0.355s
sys	0m0.013s
Test run with threshold 1048576

real	0m0.217s
user	0m0.371s
sys	0m0.024s
Test run with threshold 524288

real	0m0.141s
user	0m0.383s
sys	0m0.033s
Test run with threshold 262144

real	0m0.144s
user	0m0.367s
sys	0m0.054s
Test run with threshold 131072

real	0m0.156s
user	0m0.390s
sys	0m0.059s
Test run with threshold 65536

real	0m0.152s
user	0m0.393s
sys	0m0.065s
Test run with threshold 32768

real	0m0.159s
user	0m0.394s
sys	0m0.083s
Test run with threshold 16384

real	0m0.175s
user	0m0.417s
sys	0m0.102s

The main parts of the merge sort computation being done in parallel are the recursive sorts for 
each half of the data array. These are done through child processes, which can run in parallel (from the 
viewpoint of the OS Kernel) because there are no dependencies between them, since they are accessing different 
parts of the data array. This allows the OS Kernel to schedule their operations on different CPU cores. 

In our testing of various threshold values, our program exhibited parallel speedup, where the time needed 
to complete the merge sort began to decrease as the threshold decreased as more parallel processes were used 
to sort the array. However, we noticed that at a certain "critical" threshold, the time needed to complete the sort 
no longer had a significant decrease. We hypothesize that this could be due to the limited number of CPU cores available to 
run parallel processes on. This means that the OS Kernel essentially has to "wait" on previously executing processes in order to
continue to the next ones. 