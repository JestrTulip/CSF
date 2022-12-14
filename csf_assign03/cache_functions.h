#ifndef CACHE_FUNCTIONS_H
#define CACHE_FUNCTIONS_H
#include <stdint.h>
#include <vector>
#include <map>
#include <tuple> 
#include <utility>
#include <string>
#include <iostream>
#include <cmath>
#include <sstream>
#include <cstring>

/* Struct to model blocks in a cache
 * 
 * Fields:
 *  dirty - tag for indetifiying whether block need to by stored on eviction
 *  load_ts - time stamp measuring time since loading
 *  access_ts - time stamp measuring time since last access
 */
struct Slot { 
    bool dirty; 
    uint32_t load_ts, access_ts;
}; 

/* Struct to model sets in a cache
 * 
 * Fields:
 *  slots - map containing tags as keys linked to corresponding slots
 */
struct Set {
    //possibly a map of tag to index of efficiency 
    std::map<uint32_t, Slot> slots;
}; 

/* Struct to cache
 * 
 * Fields:
 *  sets - vector containing sets in the cache
 */
struct Cache { 
    std::vector<Set> sets; 
}; 

/* Function to poulate cache with empty sets
 * 
 * Parameters:
 *  set_num - number of sets in the cache
 * 
 * Returns:
 *  Cache with certain number of sets
 */
Cache populate_cache(uint32_t set_num); 


int check_error_conditions(int argc, char **argv, int set_num, int block_num, int block_size);

/* Function process input line and return necessary values
 * 
 * Parameters:
 *  line - input line to process
 * 
 * Returns:
 *  Pair containing letter and address contained in line
 */
std::pair<std::string, std::uint64_t> read_line(std::string line); 


/* Function to check if a value is a power of 2
 * 
 * Parameters:
 *  val - value to check
 * 
 * Returns:
 *  True if value is a power of 2, false if not
 */
int check_power_of_2(int val);


/* Function to get tag from an address for a certain cache
 * 
 * Parameters:
 *  address - address to derive tag from
 *  set_num - number of sets in the cache that the address is for
 *  block_size - size of blocks in the cache that the address is for
 * 
 * Returns:
 *  tag for a certain cache
 */
uint32_t get_tag(uint32_t address, uint32_t set_num, uint32_t block_size); 


/* Function to get index from an address for a certain cache
 * 
 * Parameters:
 *  address - address to derive index from
 *  set_num - number of sets in the cache that the address is for
 *  block_size - size of blocks in the cache that the address is for
 * 
 * Returns:
 *  index for a certain cache
 */
uint32_t get_index(uint32_t address, uint32_t set_num, uint32_t block_size);


/* Function to perform a store to cache
 * 
 * Parameters:
 *  cache - address of cache to manipulate
 *  address - address for store to occur for
 *  set_num - number of sets in the cache
 *  block_num - number of blocks in a set
 *  block_size - size of blocks in the cache
 *  write_allocate - true if cache is write-allocate, false if cache is no-write-allocate
 *  write_through - true if cache is write through, fale is cache is write back
 *  lru - true if cache is lru, false if cache is fifo
 *  timestamp - current program time
 * 
 * Returns:
 *  tuple containing updated store hits, store misses, and cycles
 */
std::tuple<uint32_t, uint32_t, uint32_t> store_to_cache(Cache & cache, uint32_t address, uint32_t set_num, uint32_t block_num, uint32_t block_size, bool write_allocate, bool write_through,  bool lru, uint32_t timestamp);


/* Function to perform a load to cache
 * 
 * Parameters:
 *  cache - address of cache to manipulate
 *  address - address for store to occur for
 *  set_num - number of sets in the cache
 *  block_num - number of blocks in a set
 *  block_size - size of blocks in the cache
 *  lru - true if cache is lru, false if cache is fifo
 *  timestamp - current program time
 * 
 * Returns:
 *  tuple containing updated load hits, load misses, and cycles
 */
std::tuple<uint32_t, uint32_t, uint32_t> load_to_cache(Cache & cache, uint32_t address, uint32_t set_num, uint32_t block_num, uint32_t block_size, bool lru, uint32_t timestamp);

/* Function to perform a load to cache for a non-dirty slot
 * 
 * Parameters:
 *  cache - address of cache to manipulate
 *  address - address for store to occur for
 *  set_num - number of sets in the cache
 *  block_num - number of blocks in a set
 *  block_size - size of blocks in the cache
 *  lru - true if cache is lru, false if cache is fifo
 *  timestamp - current program time
 * 
 * Returns:
 *  tuple containing updated load hits, load misses, and cycles
 */
uint32_t write_allocate_load(Cache & cache, uint32_t adresss, uint32_t set_num, uint32_t block_num, uint32_t block_size, bool lru, uint32_t timestamp);


/* Function to perform a load to cache for a dirty slot
 * 
 * Parameters:
 *  cache - address of cache to manipulate
 *  address - address for store to occur for
 *  set_num - number of sets in the cache
 *  block_num - number of blocks in a set
 *  block_size - size of blocks in the cache
 *  lru - true if cache is lru, false if cache is fifo
 *  timestamp - current program time
 * 
 * Returns:
 *  tuple containing updated load hits, load misses, and cycles
 */
uint32_t write_allocate_dirty_load(Cache &cache, uint32_t address, uint32_t set_num, uint32_t block_num, uint32_t block_size, bool lru, uint32_t timestamp);

#endif // CACHE_FUNCTIONS_H