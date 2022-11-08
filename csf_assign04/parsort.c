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
  int mid = (begin + end) / 2; 
  if((end-begin) <= threshold){
    qsort(arr, end-begin+1, sizeof(uint64_t), cmpfunc);
  } else{
    pid_t pid = fork(); 
    if (pid == -1) {
      //fork failed, handle error and exit
      fprintf(stderr, "Error: fork failed!\n");
      return; 
    } else if (pid == 0) {
      if (begin < end) {
        merge_sort(arr, begin, mid, threshold); 
        exit(0);
        //merge_sort(arr, mid + 1, end, threshold); needs to be a diff forl 
      }
    } else { 
      int wstatus; 
      pid_t actual_pid = waitpid(pid, &wstatus, 0); 
      if (actual_pid == -1) {
        //handle failure
      }
    }

    pid = fork(); 
    if (pid == -1) {
      fprintf(stderr, "Error: fork failed!\n");
      return; 
    } else if (pid == 0) {
      if (begin < end) {
        merge_sort(arr, mid + 1, end, threshold); 
        exit(0); 
      }
    }
    

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
    fprintf(stderr, "mmap faled to create new mapping\n");
    return 4; 
  }


  // TODO: sort the data!
  merge_sort(data, 0, sizeof(data)/sizeof(uint64_t)-1, threshold);


  // TODO: unmap and close the file
  munmap(data, file_size_in_bytes);
  close(fd);
  
  // TODO: exit with a 0 exit code if sort was successful
  return 0;
}
