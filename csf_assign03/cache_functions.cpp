#include <iostream>
#include "cache_functions.h"
#include <cmath>


int check_power_of_2(int val){
    if(ceil(log2(val))== floor(log2(val))) { return 1;}
	else { return 0;}
}