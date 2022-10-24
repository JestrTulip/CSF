#ifndef CACHE_FUNCTIONS_H
#define CACHE_FUNCTIONS_H
#include <stdint.h>
#include <vector>


struct Slot { 
    uint32_t tag;
    bool valid;
    uint32_t load_ts, access_ts;
}; 

struct Set {
    std::vector<Slot> slots;
}; 

struct Cache { 
    std::vector<Set> sets; 
}; 

int check_power_of_2(int val);

uint32_t get_tag(uint32_t address, uint32_t set_num, uint32_t block_size); 

uint32_t get_index(uint32_t address, uint32_t set_num, uint32_t block_size); 

bool store_to_cache(uint32_t address, uint32_t set_num, uint32_t block_size, bool write_allocate, bool write_through,  bool lru);

void load_to_cache(uint32_t address, uint32_t set_num, uint32_t block_size, bool lru);

#endif // CACHE_FUNCTIONS_H