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

Cache populate_cache(uint32_t set_num) {
    
    Set sets;
    
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
    if(set_num == 1){
        return address >> (uint32_t) (log2(block_size));
    } else {
        return address >> (uint32_t) (log2(set_num) + log2(block_size));
    }
    
}

uint32_t get_index(uint32_t address, uint32_t set_num, uint32_t block_size){
    if(set_num == 1){
        return 0;
    } else {
        address = address << (uint32_t) (32 - log2(set_num) - log2(block_size));
        return address >> (uint32_t) (32 - log2(set_num));
    }
    
}


std::tuple<uint32_t, uint32_t, uint32_t> store_to_cache(Cache & cache, uint32_t address, uint32_t set_num, uint32_t block_num, uint32_t block_size, bool write_allocate, bool write_through,  bool lru, uint32_t timestamp){
    uint32_t storeHit = 0;
    uint32_t storeMiss = 0;
    uint32_t cycles = 0;

    uint32_t currtag = get_tag(address, set_num, block_size);
    uint32_t currindex = get_index(address, set_num, block_size);
    
    //can get set using Set &s = cache.sets[index];
    
    if(!(cache.sets[currindex].slots.find(currtag)== cache.sets[currindex].slots.end())){
        storeHit = 1;
        cache.sets[currindex].slots[currtag].access_ts = timestamp;
        if(write_through){
                cycles += 100;
        } else {
            cache.sets[currindex].slots[currtag].dirty = 1;
        } 
    }

        if(!storeHit){

            storeMiss++; 
            
            if(write_allocate){
                if(write_through){
                    cycles += write_allocate_load(cache, address, set_num, block_num, block_size, lru, timestamp);
                    cycles += 100;
                } else {
                    cycles += write_allocate_dirty_load(cache, address, set_num, block_num, block_size, lru, timestamp);
                }
            } else {
                cycles += 100;
            }
        }
        
    
    return {storeHit, storeMiss, cycles};
}

std::tuple<uint32_t, uint32_t, uint32_t> load_to_cache(Cache & cache, uint32_t address, uint32_t set_num, uint32_t block_num, uint32_t block_size, bool lru, uint32_t timestamp){
    uint32_t loadHit = 0;
    uint32_t loadMiss = 0;
    uint32_t cycles = 0;

    uint32_t evicted = 0;
    uint32_t min_ts = -1;

    uint32_t currtag = get_tag(address, set_num, block_size);
    uint32_t currindex = get_index(address, set_num, block_size);

    if(!(cache.sets[currindex].slots.find(currtag)== cache.sets[currindex].slots.end())){
        loadHit = 1;
        cache.sets[currindex].slots[currtag].access_ts = timestamp;
    }

    if(!loadHit){

        loadMiss++; 

        if(lru){
            if(cache.sets[currindex].slots.size() >= block_num) {
                for (auto it = cache.sets[currindex].slots.begin(); it != cache.sets[currindex].slots.end(); ++it) {
                    //find slot with lowest access timestamp
                    if(it->second.access_ts < min_ts){
                        min_ts = it->second.access_ts;
                        evicted = it->first;
                    }
                }
                if(cache.sets[currindex].slots[evicted].dirty) { 
                    cycles += 100 * block_size / 4;
                }
                cache.sets[currindex].slots.erase(evicted);
            }


            Slot insert = {0, 0, timestamp};
            cache.sets[currindex].slots.insert({currtag, insert});
            cycles += 100 * block_size / 4;
        } else {
            if(cache.sets[currindex].slots.size() >= block_num) {
                for (auto it = cache.sets[currindex].slots.begin(); it != cache.sets[currindex].slots.end(); ++it) {
                    //find slot with lowest access timestamp
                    if(it->second.load_ts < min_ts){
                        min_ts = it->second.load_ts;
                        evicted = it->first;
                    }
                }
                if(cache.sets[currindex].slots[evicted].dirty) { 
                    cycles += 100 * block_size / 4;
                }
                cache.sets[currindex].slots.erase(evicted);
            }

            Slot insert = {0, timestamp, 0};
            cache.sets[currindex].slots.insert({currtag, insert});
            cycles += 100 * block_size / 4;    
        }

        }
    return {loadHit, loadMiss, cycles};
}




uint32_t write_allocate_load(Cache & cache, uint32_t address, uint32_t set_num, uint32_t block_num, uint32_t block_size, bool lru, uint32_t timestamp){
    uint32_t cycles = 0;
    uint32_t min_ts = -1;
    uint32_t evicted;

    uint32_t currtag = get_tag(address, set_num, block_size);
    uint32_t currindex = get_index(address, set_num, block_size);

    if(lru){

        if(cache.sets[currindex].slots.size() >= block_num) {
            for (auto it = cache.sets[currindex].slots.begin(); it != cache.sets[currindex].slots.end(); ++it) {
                //find slot with lowest access timestamp
                if(it->second.access_ts < min_ts){
                    min_ts = it->second.access_ts;
                    evicted = it->first;
                }
            }
            if(cache.sets[currindex].slots[evicted].dirty) { 
                cycles += 100 * block_size / 4;
            }
            cache.sets[currindex].slots.erase(evicted);
        }


        Slot insert = {0, 0, timestamp};
        cache.sets[currindex].slots.insert({currtag, insert});
        cycles += 100 * block_size / 4;

    } else {
        if(cache.sets[currindex].slots.size() >= block_num) {
            for (auto it = cache.sets[currindex].slots.begin(); it != cache.sets[currindex].slots.end(); ++it) {
                //find slot with lowest access timestamp
                if(it->second.load_ts < min_ts){
                    min_ts = it->second.load_ts;
                    evicted = it->first;
                }
            }
            if(cache.sets[currindex].slots[evicted].dirty) { 
                cycles += 100 * block_size / 4;
            }
            cache.sets[currindex].slots.erase(evicted);
        }

        Slot insert = {0, timestamp, 0};
        cache.sets[currindex].slots.insert({currtag, insert});
        cycles += 100 * block_size / 4; 
    }
    return cycles;
}



uint32_t write_allocate_dirty_load(Cache & cache, uint32_t address, uint32_t set_num, uint32_t block_num, uint32_t block_size, bool lru, uint32_t timestamp){
    uint32_t cycles = 0;
    uint32_t min_ts = -1;
    uint32_t evicted;

    uint32_t currtag = get_tag(address, set_num, block_size);
    uint32_t currindex = get_index(address, set_num, block_size);

    if(lru){

        if(cache.sets[currindex].slots.size() >= block_num) {
            for (auto it = cache.sets[currindex].slots.begin(); it != cache.sets[currindex].slots.end(); ++it) {
                //find slot with lowest access timestamp
                if(it->second.access_ts < min_ts){
                    min_ts = it->second.access_ts;
                    evicted = it->first;
                }
            }
            if(cache.sets[currindex].slots[evicted].dirty) { 
                cycles += 100 * block_size / 4;
            }
            cache.sets[currindex].slots.erase(evicted);
        }


        Slot insert = {0, 0, timestamp};
        cache.sets[currindex].slots.insert({currtag, insert});
        cycles += 100 * block_size / 4;

    } else {
        if(cache.sets[currindex].slots.size() >= block_num) {
            for (auto it = cache.sets[currindex].slots.begin(); it != cache.sets[currindex].slots.end(); ++it) {
                //find slot with lowest access timestamp
                if(it->second.load_ts < min_ts){
                    min_ts = it->second.load_ts;
                    evicted = it->first;
                }
            }
            if(cache.sets[currindex].slots[evicted].dirty) { 
                cycles += 100 * block_size / 4;
            }
            cache.sets[currindex].slots.erase(evicted);
        }

        Slot insert = {1, timestamp, 0};
        cache.sets[currindex].slots.insert({currtag, insert});
        cycles += 100 * block_size / 4; 
    }
    return cycles;
}