#include "cache_functions.h"


using namespace std;

int main(int argc, char **argv){
    
    //check if args are integers

    int set_num = (int) *argv[1];
    int block_num = (int) *argv[2];
    int block_size = (int) *argv[3];

    if(check_error_conditions(argc, argv, set_num, block_num, block_size)){
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


    while (getline(cin, line)) {
        pair<string, uint64_t> args = read_line(line); 
        if (args.first[0] == 's' ) {
            
        }
    }

}