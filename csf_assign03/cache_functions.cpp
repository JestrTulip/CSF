#include <iostream>
#include "cache_functions.h"
#include <cmath>


int check_power_of_2(int val){
    if(ceil(log2(val))== floor(log2(val))) { return 1;}
	else { return 0;}
}

uint32_t get_tag(uint32_t address, uint32_t set_num, uint32_t block_size){
    return address >> (set_num + block_size);
}

uint32_t get_index(uint32_t address, uint32_t set_num, uint32_t block_size){
    address = address << (32 - set_num - block_size);
    return address >> (32 - set_num);
}

bool store_to_cache(uint32_t address, uint32_t set_num, uint32_t block_size, bool write_allocate, bool write_through,  bool lru);

void load_to_cache(uint32_t address, uint32_t set_num, uint32_t block_size, bool lru);