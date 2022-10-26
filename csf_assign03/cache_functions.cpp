#include "cache_functions.h"
#include <iostream>
#include <cmath>
#include <sstream>
#include <cstring>


using std::vector; 
using std::pair;
using std::tuple; 

int check_power_of_2(int val){
    if(ceil(log2(val))== floor(log2(val))) { 
        return 1;
    }
	else { 
        return 0;
    }
}
//argc 8
int check_error_conditions(int argc, char **argv, int set_num, int block_num, int block_size) { 
    return argc != 7  || !check_power_of_2(set_num) || !check_power_of_2(block_num) || !check_power_of_2(block_size) || (block_size < 4)
        || !(!strcmp(argv[4], "write-allocate") || !strcmp(argv[4], "no-write-allocate"))
        || !(!strcmp(argv[5], "write-through") || !strcmp(argv[5], "write-back"))
        || !(!strcmp(argv[6], "lru") || !strcmp(argv[6], "fifo"));
}

Cache populate_cache(uint32_t set_num, uint32_t block_num) {
    Slot slot = {0,0,0,0,0}; 
    
    Set sets; 
    //sets.index = -1; 
    vector<Slot> slotsList(block_num);
    fill(slotsList.begin(), slotsList.end(),slot);
    sets.slots = slotsList; 
    
    vector<Set> setsList(set_num);
    fill(setsList.begin(), setsList.end(),sets);
    Cache finalCache;
    finalCache.sets = setsList;
    
    return finalCache;
}

std::pair<std::string, std::uint64_t> read_line(std::string line) {


    std::istringstream is(line);
    std::string f_field; 

    std::string action; 
    std::uint64_t block; 
    

    is >> f_field;  
    action = f_field; 
    is >> f_field; 
    block = std::stoll(f_field, nullptr, 16);

    return {action, block}; 

}

uint32_t get_tag(uint32_t address, uint32_t set_num, uint32_t block_size){
    return address >> (uint32_t) (log2(set_num) + log2(block_size));
}

uint32_t get_index(uint32_t address, uint32_t set_num, uint32_t block_size){
    address = address << (uint32_t) (32 - log2(set_num) - log2(block_size));
    return address >> (uint32_t) (32 - log2(set_num));
}


std::tuple<Cache, uint32_t, uint32_t, uint32_t> store_to_cache(Cache cache, uint32_t address, uint32_t set_num, uint32_t block_size, bool write_allocate, bool write_through,  bool lru, uint32_t timestamp){
    uint32_t storeHit = 0;
    uint32_t storeMiss = 0;
    uint32_t cycles = 0;

    uint32_t index = 0;
    uint32_t evicted = 0;
    uint32_t minaccess_ts = 100;

    uint32_t currtag = get_tag(address, set_num, block_size);
    uint32_t currindex = get_index(address, set_num, block_size);
    
    //can get set using Set &s = cache.sets[index];
    
    for (std::vector<Slot>::iterator it = cache.sets[currindex].slots.begin() ; it != cache.sets[currindex].slots.end(); ++it) {
        if(currtag == it->tag && it->valid){
            storeHit = 1;
            it->access_ts = timestamp;
            if(write_through){
                    cycles += 100;
            } else {
                it->dirty = 1;
            } 
        }
        //find slot with lowest access timestamp
        if(it->access_ts < minaccess_ts){
            minaccess_ts = it->access_ts;
            evicted = index;
        }
        index+=1;
     }

        if(!storeHit){
            storeMiss++; 
            if(write_allocate){
                if(write_through){
                    std::tuple <Cache, uint32_t> load = write_allocate_load(cache, address, set_num, block_size, lru, evicted, timestamp);
                    cycles+=100;
                    cycles += std::get<1>(load);
                    cache = std::get<0>(load);
                } else {
                    std::tuple <Cache, uint32_t> load = write_allocate_dirty_load(cache, address, set_num, block_size, lru, evicted, timestamp);
                    cycles += std::get<1>(load);
                    cache = std::get<0>(load);
                }
            } else {
                cycles += 100;
            }
        }
        
    
    return {cache, storeHit, storeMiss, cycles};
}

std::tuple<Cache, uint32_t, uint32_t, uint32_t> load_to_cache(Cache cache, uint32_t address, uint32_t set_num, uint32_t block_size, bool lru, uint32_t timestamp){
    uint32_t loadHit = 0;
    uint32_t loadMiss = 0;
    uint32_t cycles = 0;

    uint32_t index = 0;
    uint32_t evicted = 0;
    uint32_t minaccess_ts = 100;

    uint32_t currtag = get_tag(address, set_num, block_size);
    uint32_t currindex = get_index(address, set_num, block_size);

    for (std::vector<Slot>::iterator it = cache.sets[currindex].slots.begin() ; it != cache.sets[currindex].slots.end(); ++it) {
        if(currtag == it->tag && it->valid){
            it->access_ts = timestamp;
            loadHit = 1;
            cycles += 100; //modified
        }
        //find slot with lowest access timestamp
        if(it->access_ts < minaccess_ts){
            minaccess_ts = it->access_ts;
            evicted = index;
        }
        index+=1;
    }

    //tag not found so a slot must be evicted
    if(!loadHit && lru){
        loadMiss = 1;
        if(cache.sets[currindex].slots[evicted].dirty) { 
            cycles += 100; //modified
        }
        cache.sets[currindex].slots[evicted].tag = currtag;
        cache.sets[currindex].slots[evicted].valid = 1;
        cache.sets[currindex].slots[evicted].load_ts = 0;
        cache.sets[currindex].slots[evicted].access_ts = timestamp;
        cache.sets[currindex].slots[evicted].dirty = 0;
        cycles += 100 * block_size / 4;

    }
    return {cache, loadHit, loadMiss, cycles};
}

std::tuple<Cache, uint32_t> write_allocate_load(Cache cache, uint32_t address, uint32_t set_num, uint32_t block_size, bool lru, uint32_t evicted, uint32_t timestamp){
    uint32_t cycles = 0;

    uint32_t currtag = get_tag(address, set_num, block_size);
    uint32_t currindex = get_index(address, set_num, block_size);

    //tag not found so a slot must be evicted
    if(lru){
        if(cache.sets[currindex].slots[evicted].dirty) { 
            cycles += 100; //modified
        }
        cache.sets[currindex].slots[evicted].tag = currtag;
        cache.sets[currindex].slots[evicted].valid = 1;
        cache.sets[currindex].slots[evicted].load_ts = 0;
        cache.sets[currindex].slots[evicted].access_ts = timestamp;
        cache.sets[currindex].slots[evicted].dirty = 0;
        cycles += 100 * block_size / 4;
    }

    return {cache, cycles};
}



std::tuple<Cache, uint32_t> write_allocate_dirty_load(Cache cache, uint32_t address, uint32_t set_num, uint32_t block_size, bool lru, uint32_t evicted, uint32_t timestamp){
    uint32_t cycles = 0;

    uint32_t currtag = get_tag(address, set_num, block_size);
    uint32_t currindex = get_index(address, set_num, block_size);

    //tag not found so a slot must be evicted
    if(lru){
        if(cache.sets[currindex].slots[evicted].dirty) { 
            cycles += 100; //modified 
        }
        cache.sets[currindex].slots[evicted].tag = currtag;
        cache.sets[currindex].slots[evicted].valid = 1;
        cache.sets[currindex].slots[evicted].load_ts = 0;
        cache.sets[currindex].slots[evicted].access_ts = timestamp;
        cache.sets[currindex].slots[evicted].dirty = 1;
        cycles += 100 * block_size / 4;
    }

    return {cache, cycles};
}