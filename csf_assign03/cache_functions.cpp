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

bool store_to_cache(uint32_t address, uint32_t set_num, uint32_t block_size, bool write_allocate, bool write_through,  bool lru);

void load_to_cache(uint32_t address, uint32_t set_num, uint32_t block_size, bool lru);

