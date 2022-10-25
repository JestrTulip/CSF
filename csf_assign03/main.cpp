#include "cache_functions.h"
#include <iostream>
#include <cstring>


using namespace std;

int main(int argc, char **argv){
    
    //check if args are integers

    int set_num = (int) *argv[1];
    int block_num = (int) *argv[2];
    int block_size = (int) *argv[3];

    if(argc !=7 || !check_power_of_2(set_num) || !check_power_of_2(block_num) || !check_power_of_2(block_size) || (block_size < 4)
        || !(!strcmp(argv[4], "write_allocate") || !strcmp(argv[4], "no_write_allocate"))
        || !(!strcmp(argv[5], "write_through") || !strcmp(argv[5], "write_back"))
        || !(!strcmp(argv[6], "lru") || !strcmp(argv[6], "fifo"))){
            cerr << "ERROR: Invalid cache design parameters"; 
        return 0;
    }

    if(!strcmp(argv[4], "no_write_allocate") && !strcmp(argv[5], "write_back")){
        cerr << "Error: Invalid design parameter combination"; 
        return 0;
    }


    bool write_allocate = !strcmp(argv[4], "write_allocate");
    bool write_through = !strcmp(argv[5], "write_through");
    bool lru = !strcmp(argv[6], "lru");

    string line; 

    std::tuple<uint32_t, uint32_t, uint32_t> stats;
    uint32_t load_hits = 0;
    uint32_t load_misses = 0;
    uint32_t store_hits = 0;
    uint32_t store_misses = 0;
    uint32_t cycles_total = 0;


    Cache cache = populate_cache(set_num, block_size);

    while (getline(cin, line)) {
        pair<string, uint64_t> args = read_line(line); 
        if (args.first[0] == 's' ) {
            stats = store_to_cache(cache, args.second, set_num, block_size, write_allocate, write_through, lru);
            store_hits += std::get<0>(stats);
            store_misses += std::get<1>(stats);
            cycles_total += std::get<2>(stats);
        } else if (args.first[0] == 'l' ) {
            stats = load_to_cache(cache, args.second, set_num, block_size, lru);
            load_hits += std::get<0>(stats);
            load_misses += std::get<1>(stats);
            cycles_total += std::get<2>(stats);
        }
    }
    printf("Total loads: %u\nTotal stores: %n\nLoad hits: %n\nLoad misses: %n\nStore hits: %n\nStore misses: %n\nTotal cycles: %d\n", 
    load_hits+load_misses, store_hits+store_misses, load_hits, load_misses, store_hits, store_misses, cycles_total);
}