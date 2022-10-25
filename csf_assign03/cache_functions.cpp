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

int check_error_conditions(int argc, char **argv, int set_num, int block_num, int block_size) { 
    return argc !=7 || !check_power_of_2(set_num) || !check_power_of_2(block_num) || !check_power_of_2(block_size) || (block_size < 4)
        || !(!strcmp(argv[4], "write_allocate") || !strcmp(argv[4], "no_write_allocate"))
        || !(!strcmp(argv[5], "write_through") || !strcmp(argv[5], "write_back"))
        || !(!strcmp(argv[6], "lru") || !strcmp(argv[6], "fifo"));
}

Cache populate_cache(uint32_t set_num, uint32_t block_size) {
    Slot slot = {0,0,0,0}; 
    
    Set sets; 
    sets.index = -1; 
    vector<Slot> slotsList(block_size);
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
    block = std::stoi(f_field, nullptr, 256);
    is >> f_field;

    return {action, block}; 
}

uint32_t get_tag(uint32_t address, uint32_t set_num, uint32_t block_size){
    return address >> (set_num + block_size);
}

int32_t get_index(uint32_t address, uint32_t set_num, uint32_t block_size){
    address = address << (32 - set_num - block_size);
    return address >> (32 - set_num);
}


std::tuple<uint32_t, uint32_t, uint32_t> store_to_cache(Cache cache, uint32_t address, uint32_t set_num, uint32_t block_size, bool write_allocate, bool write_through,  bool lru){
    uint32_t storeHit = 0;
    uint32_t storeMiss = 0;
    uint32_t cycles = 0;

    uint32_t currtag = get_tag(address, set_num, block_size);
    int32_t currindex = get_index(address, set_num, block_size);

    incrementTime(cache);

    for (std::vector<Set>::iterator it = cache.sets.begin() ; it != cache.sets.end(); ++it) {
        if(currindex == it->index) {
            for (std::vector<Slot>::iterator it2 = it->slots.begin() ; it2 != it->slots.end(); ++it2) {
                if(currtag == it2->tag){
                    if(write_through){
                        it2->access_ts = 0;
                        storeHit = 1;
                        cycles += 100;
                    } else {
                        it2->valid = 0;
                    } 
                }
            }

            if(!storeHit){
                if(write_allocate){
                    std::tuple <uint32_t, uint32_t, uint32_t> load = load_to_cache(cache, address, set_num, block_size, lru);
                    cycles += std::get<2>(load);
                } else {
                    cycles += 100 * block_size / 4;
                }
            }
        }
    }
    return {storeHit, storeMiss, cycles};
}

std::tuple<uint32_t, uint32_t, uint32_t> load_to_cache(Cache cache, uint32_t address, uint32_t set_num, uint32_t block_size, bool lru){
    uint32_t loadHit = 0;
    uint32_t loadMiss = 0;
    uint32_t cycles = 0;

    std::vector<Slot>::iterator evicted;
    uint32_t maxaccess_ts = 0;

    uint32_t currtag = get_tag(address, set_num, block_size);
    int32_t currindex = get_index(address, set_num, block_size);
    
    

    //find the set with the correct index
    // iterate through the slots that the set has for the right tag 
    // if you find the tag, load hit = update access ts.
    // if load miss, add cycles (100 * size bytes / 4), try to add to cache by finding empty slot, else evict (set valid to true and update load ts)
    // for lru, find the access timestamp and find the least recently used, (smallest time), mark as dirty and evict, 

    incrementTime(cache);

    for (std::vector<Set>::iterator it = cache.sets.begin() ; it != cache.sets.end(); ++it) {
        if(currindex == it->index || it->index < 0) {
            for (std::vector<Slot>::iterator it2 = it->slots.begin() ; it2 != it->slots.end(); ++it2) {
                if(currtag == it2->tag){
                    it2->access_ts = 0;
                    loadHit = 1;
                    cycles += 100 * block_size / 4;
                }
            }

            //tag not found so a slot must be evicted
            if(!loadHit && lru){
                for (std::vector<Slot>::iterator it2 = it->slots.begin() ; it2 != it->slots.end(); ++it2) {
                    if(it2->access_ts > maxaccess_ts){
                        maxaccess_ts = it2->access_ts;
                        evicted = it2;
                    }
                }
                if(!evicted->valid) { cycles += 100 * block_size / 4;} //add to cycles
                evicted->tag = currtag;
                evicted->load_ts = 0;
            }
        }
    }
    return {loadHit, loadMiss, cycles};
}

void incrementTime(Cache cache){
    for (std::vector<Set>::iterator it = cache.sets.begin() ; it != cache.sets.end(); ++it) {
        for (std::vector<Slot>::iterator it2 = it->slots.begin() ; it2 != it->slots.end(); ++it2) {
            it2->load_ts+=1;
            it2->access_ts+=1;
        }
    }
}

