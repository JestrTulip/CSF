#include "cache_functions.cpp"
#include <iostream>
#include <cstring>

using namespace std;

int main(int argc, char **argv){

    if(argc !=7 || !check_power_of_2(argc[1]) || !check_power_of_2(argc[2]) || !check_power_of_2(argc[3]) || argc[3] < 4
        || !(!strcmp(argv[4], "write_allocate") || !strcmp(arg[4], "no_write_allocate"))
        || !(!strcmp(argv[5], "write_through") || !strcmp(arg[5], "write_back"))
        || !(!strcmp(argv[6], "lru") || !strcmp(argv[6], "write fifo"))){
        return 0;
    }

    if(!strcmp(arg[4], "no_write_allocate") && !strcmp(arg[5], "write_back")){
        return 0;
    }

    int set_num = argv[1];
    int block_num = argv[2];
    int block_size = argv[3];
}