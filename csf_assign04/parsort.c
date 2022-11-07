#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

void merge(int64_t *arr, size_t begin, size_t mid, size_t end, int64_t *temparr) {
  // TODO: implement
  size_t counter;
  size_t midplus = mid + 1;
  while(begin <= mid && midplus <= end){
    if(arr[begin] < arr[midplus]){
      temparr[counter] = arr[begin];
      begin++;
    } else {
      temparr[counter] = arr[midplus];
      midplus++;
    }
    counter++;
  }

  while(begin <= mid){
    temparr[counter] = arr[begin];
    begin++;
    counter++;
  }

  while(midplus <= end){
    temparr[counter] = arr[midplus];
    midplus++;
    counter++;
  }
}

void merge_sort(int64_t *arr, size_t begin, size_t end, size_t threshold) {
  // TODO: implement
  if((end-begin) <= threshold){
    qsort(arr, end-begin+1, sizeof(uint64_t), cmpfunc);
  }
}

int main(int argc, char **argv) {
  // check for correct number of command line arguments
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <filename> <sequential threshold>\n", argv[0]);
    return 1;
  }

  // process command line arguments
  const char *filename = argv[1];
  char *end;
  size_t threshold = (size_t) strtoul(argv[2], &end, 10);
  if (end != argv[2] + strlen(argv[2]))
    /* TODO: report an error (threshold value is invalid) */;

  // TODO: open the file

  // TODO: use fstat to determine the size of the file

  // TODO: map the file into memory using mmap

  // TODO: sort the data!

  // TODO: unmap and close the file

  // TODO: exit with a 0 exit code if sort was successful
}
