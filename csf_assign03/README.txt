Authors: Dayann D'almeida - dkouako1
Kedar Krishnan - kkrish13

Milestone 1: Kedar worked on implementation, Dayann worked on bug fixes for the Makefile.

Milestone 2: Dayann worked on input validation and population of slots and cache, Kedar implemented main 
funtionality for lru. Both worked on the debugging. 

Milestone 3: 

All tests involving LRU were done by Kedar and all test involving FIFO were done by Dayann.

The following experiments were used to determine the most ideal cache configuration:
1) Varying Cache sizes
    We decided to test 3 different cache sizes. If cache size is too small then it may be time conusuming accessing/ moving data into memory. Alternatively, if a 
    cache is too large then it may be wastefull to load certain data into the cache as it may never get accessed again.
2) Varying number of sets and number of slots
    Withing each cache size we tested up to 8 different combinations of set numbers and slot numbers going from a direct-mapped cache to a fully associative cache.
    A fully associative cache is much less likely to experience the need to evict slots, which can lead to a lower miss rate. However, a direct-mapped cache
    can be faster if there are fewer misses. For this reason, n-way set intermediates were also tested to find the optimal configuration for speed and lower miss rate.
3) Varying slot size
    For each set number / slot number configuration we also varied the slot size, in increments from 4 to 256 bytes. A larger block size exploit spatial locality and
    can lead to lower miss rates. However, when they are stored or loaded they require much more time (ie. miss penalty) and they can lead to wasted space. 
    Alternatively, a smaller block size can be more efficient for frequent small stores and loads as they do not have the same penalty as the large blocks.
4) Verying cache policies
    We tested all possibe combinations for cache policies, namely write-through/write-allocate, write-through/no-write-allocate, and write-back/write-allocate. 
    This allowed us to model all relevant possibilities for how the simulator would react when faced with a hit/miss, and to generate a more variable set 
    of reports. 


Results: 
For both FIFO and LRU , write-back and write-allocate were shown to have low miss rates and low number of cycles.

Larger cache sizes tended to display lower miss rates and lower cycle times, however, there seemed to be a point at which their efficiency would plateau, namely 
when increasing the number of slots. Generally, a lower number of sets yielded more favorable results. 

Larger block sizes tended to marginally increase the number of cycles/ However, they had a significantly lower miss rate for loads and stores.

Overall the best cache configuration was found to be a set associative cache with a lower set number, higher block number, high block size with write-back, write-allocate, lru 
in terms of cycles, hit rate, and miss rate. This configuration was the following:
16 64 256 write-allocate write-back lru

