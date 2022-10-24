#include "cache_functions.h"
#include <iostream>
#include <cmath>
#include <sstream>


int check_power_of_2(int val){
    if(ceil(log2(val))== floor(log2(val))) { 
        return 1;
    }
	else { 
        return 0;
    }
}

std::pair<std::string, std::uint64_t> read_line(std::string line) {
    std::istringstream is(line);
    std::string f_field; 

    std::string action; 
    std::uint64_t block; 
    int val; 

    is >> f_field;  
    action = f_field; 
    is >> f_field; 
    block = std::stoi(f_field, nullptr, 256);
    is >>f_field;
    val = std::stoi(f_field); 

    return {action, block}; 
}

uint32_t get_tag(uint32_t address, uint32_t set_num, uint32_t block_size){
    return address >> (set_num + block_size);
}

uint32_t get_index(uint32_t address, uint32_t set_num, uint32_t block_size){
    address = address << (32 - set_num - block_size);
    return address >> (32 - set_num);
}

bool store_to_cache(Cache cache, uint32_t address, uint32_t set_num, uint32_t block_size, bool write_allocate, bool write_through,  bool lru);

uint32_t load_to_cache(Cache cache, uint32_t address, uint32_t set_num, uint32_t block_size, bool lru){
    uint32_t loadStatus = 0;
    std::vector<Slot>::iterator evicted;
    uint32_t maxload_ts = 0;
    uint32_t currtag = get_tag(address, set_num, block_size);
    uint32_t currindex = get_index(address, set_num, block_size);
    for (std::vector<Set>::iterator it = cache.sets.begin() ; it != cache.sets.end(); ++it) {
        if(currindex == it->index) {
            for (std::vector<Slot>::iterator it2 = it->slots.begin() ; it2 != it->slots.end(); ++it2) {
                if(it2->load_ts > maxload_ts) { evicted = it2; }
            
                it2->load_ts+=1;
                it2->access_ts+=1;
                if(currtag == it2->tag){
                    it2->load_ts = 0;
                    it2->access_ts = 0;
                    loadStatus = 1;
                }
            }

            //tag not found so a slot must be evicted
            if(!loadStatus){
                evicted->tag = currtag;
                evicted->load_ts = 0;
                evicted->access_ts = 0;
            }
        }
    }
    return loadStatus;
}

