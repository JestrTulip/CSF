#include "cache_functions.h"


using namespace std;

int main(int argc, char **argv){
    
    //check if args are integers

    int set_num = stoi(argv[1]);
    int block_num = stoi(argv[2]);
    int block_size =  stoi(argv[3]);

    if(check_error_conditions(argc, argv, set_num, block_num, block_size)){
        cerr << "ERROR: Invalid cache design parameters"; 
        return 0;
    }

    if(!strcmp(argv[4], "no-write-allocate") && !strcmp(argv[5], "write-back")){
        cerr << "Error: Invalid design parameter combination"; 
        return 0;
    }


    bool write_allocate = !strcmp(argv[4], "write-allocate");
    bool write_through = !strcmp(argv[5], "write-through");
    bool lru = !strcmp(argv[6], "lru");

    string line; 

    std::tuple<uint32_t, uint32_t, uint32_t> stats;
    uint32_t load_hits = 0;
    uint32_t load_misses = 0;
    uint32_t store_hits = 0;
    uint32_t store_misses = 0;
    uint32_t cycles_total = 0;


    Cache cache = populate_cache(set_num, block_num);
    uint32_t timestamp = 1;

    while (getline(cin, line)) {
        if (line.length() == 0) { 
            break; 
        }
        pair<string, uint64_t> args = read_line(line); 
        if (args.first[0] == 's' ) {
            stats = store_to_cache(cache, args.second, set_num, block_size, write_allocate, write_through, lru, timestamp);
            store_hits += std::get<0>(stats);
            store_misses += std::get<1>(stats);
            cycles_total += std::get<2>(stats);
        } else if (args.first[0] == 'l' ) {
            stats = load_to_cache(cache, args.second, set_num, block_size, lru, timestamp);
            load_hits += std::get<0>(stats);
            load_misses += std::get<1>(stats);
            cycles_total += std::get<2>(stats);
        }
        timestamp += 1;
    }
    printf("Total loads: %u\nTotal stores: %u\nLoad hits: %u\nLoad misses: %u\nStore hits: %u\nStore misses: %u\nTotal cycles: %d\n", 
    load_hits+load_misses, store_hits+store_misses, load_hits, load_misses, store_hits, store_misses, cycles_total);
}