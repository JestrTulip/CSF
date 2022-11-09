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
  int64_t A = *(const int64_t*) a; 
  int64_t B = *(const int64_t*) b; 
  
  if (A < B) { return -1;} 

  if (A > B) { return 1; }

  return 0; 
  
}

void merge(int64_t *arr, size_t begin, size_t mid, size_t end, int64_t *temparr) { //segfault
  // don't use inclusive endpoints, you wouldn't have to do mid + 1
  size_t counter = 0;
  
  size_t second_ind = mid;
  size_t first_ind = begin;

  while(first_ind < mid && second_ind <=end){
    if(arr[first_ind] < arr[second_ind]){
      temparr[counter] = arr[first_ind];
      first_ind++;
    } else {
      temparr[counter] = arr[second_ind];
      second_ind++;
    }
    counter++;
  }

  while(first_ind < mid){
    temparr[counter] = arr[first_ind]; //segfault
    first_ind++;
    counter++;
  }

  while(second_ind < end){
    temparr[counter] = arr[second_ind];
    second_ind++;
    counter++;
  }
}

int merge_sort(int64_t *arr, size_t begin, size_t end, size_t threshold) {
  

  int64_t length = (end - begin);
  
  /* 
  mid = begin + length/2;
  */
  int mid = begin + length / 2; 
  assert(begin < mid);
  assert(mid < end);
  
  if((length) <= threshold){
    qsort(arr + begin, length, sizeof(int64_t), cmpfunc);
    return 0;
  }
  merge_sort(arr, begin, mid, threshold); 
  merge_sort(arr, mid, end, threshold); 

  
  int64_t * temparr = (int64_t *) malloc(2 * length * sizeof(int64_t)); 

  merge(arr, begin, mid, end, temparr); 
  
  for(int i = 0; i < length; ++i){
    arr[i] = temparr[i];
  }

  free(temparr);
  
  return 0; 

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
  if (end != argv[2] + strlen(argv[2])) {
    /* TODO: report an error (threshold value is invalid) */;
    fprintf(stderr, "Error: threshold value is invalid\n");
    return 1; 
  }


  // TODO: open the file
  int fd = open(filename, O_RDWR);
  if (fd < 0) {
    fprintf(stderr, "Error: could not open file %s\n", filename);
    return 2; 
  }


  // TODO: use fstat to determine the size of the file
  struct stat statbuf;
  int rc = fstat(fd, &statbuf);
  if (rc != 0) {
    fprintf(stderr, "Error: fsat could not properly get information\n");
    return 3; 
  }
  size_t file_size_in_bytes = statbuf.st_size;


  // TODO: map the file into memory using mmap
  int64_t *data = mmap(NULL, file_size_in_bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (data == MAP_FAILED) {
    fprintf(stderr, "Error: mmap faled to create new mapping\n");
    return 4; 
  }


  // TODO: sort the data!
  merge_sort(data, 0, file_size_in_bytes / sizeof(int64_t), threshold);


  // TODO: unmap and close the file
  int status = munmap(data, file_size_in_bytes);
  if(status != 0){
    fprintf(stderr, "Error: munmap faled to unmap mapping\n");
    return 5; 
  }

  status = close(fd);
  if(status != 0){
    fprintf(stderr, "Error: could not close file\n");
    return 6; 
  }
  
  // TODO: exit with a 0 exit code if sort was successful
  
  return 0;
}
