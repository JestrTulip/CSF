#include "cache_functions.h"
#include <iostream>
#include <cstring>

using namespace std;

int main(int argc, char **argv){
    
    int set_num = (int) *argv[1];
    int block_num = (int) *argv[2];
    int block_size = (int) *argv[3];

    if(argc !=7 || !check_power_of_2(set_num) || !check_power_of_2(block_num) || !check_power_of_2(block_size) || (block_size < 4)
        || !(!strcmp(argv[4], "write_allocate") || !strcmp(argv[4], "no_write_allocate"))
        || !(!strcmp(argv[5], "write_through") || !strcmp(argv[5], "write_back"))
        || !(!strcmp(argv[6], "lru") || !strcmp(argv[6], "write fifo"))){
        return 0;
    }

    if(!strcmp(argv[4], "no_write_allocate") && !strcmp(argv[5], "write_back")){
        return 0;
    }


}