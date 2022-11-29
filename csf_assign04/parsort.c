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

/** 
 * Function that compares tow values together 
 * Returns negative if first value is less than second 
 * Returns positive if second is less than first
 * Returns 0 if equal
**/ 
int cmpfunc (const void * a, const void * b) {
  
  
  int64_t A = *(const int64_t*) a; 
  int64_t B = *(const int64_t*) b; 
  
  if (A < B) { return -1;} 

  if (A > B) { return 1; }

  return 0; 
  
}

void merge(int64_t *arr, size_t begin, size_t mid, size_t end, int64_t *temparr) { //segfault
  //using inclusive endpoints 
  size_t counter = 0;
  

  size_t second_ind = mid;
  size_t first_ind = begin;

  //while first and second half have unmerged values, iterated
  while(first_ind < mid && second_ind < end){
    if(arr[first_ind] < arr[second_ind]){
      temparr[counter] = arr[first_ind];
      first_ind++;
    } else {
      temparr[counter] = arr[second_ind];
      second_ind++;
    }
    counter++;
  }

  //while first half not sorted
  while(first_ind < mid){
    temparr[counter] = arr[first_ind]; 
    first_ind++;
    counter++;
  }
  //while second not sorted
  while(second_ind < end){
    temparr[counter] = arr[second_ind];
    second_ind++;
    counter++;
  }
}

void merge_sort(int64_t *arr, size_t begin, size_t end, size_t threshold) {
  
  //flag to check that errors propagate upwards 
  int prob_checker = 0; 

  int64_t length = (end - begin);

  int mid = begin + length / 2; 
  
  if((length) <= threshold){
    qsort(arr + begin, length, sizeof(int64_t), cmpfunc);
    return; 
  } else { //greater than threshold, recursively call merge with child process
    pid_t pid = fork(); //create first child process
    if (pid == -1) {
      fprintf(stderr, "Error: fork failed\n"); 
      exit(-1); //exit with error code
    } else if(pid == 0) {
      merge_sort(arr, begin, mid, threshold); 
      exit(0);
    }

    pid_t pid2 = fork(); //create second child process
    if (pid2 == -1) {
      fprintf(stderr, "Error: fork failed\n");
      exit(-1); 
    } else if(pid2 == 0) {
      merge_sort(arr, mid, end, threshold); 
      exit(0); 
    }

    int wstatus; //wait for child process completion
    pid_t actual_pid = waitpid(pid, &wstatus, 0); 
    if (actual_pid == -1) {
      fprintf(stderr, "Error: waitpid1 failed!\n"); 
      prob_checker = 1; 
    }

    if (!WIFEXITED(wstatus)) {
      fprintf(stderr, "Error: subprocess1 crashed, was interrupted, or did not exit normally\n");
      prob_checker = 1; 
    }
    if (WEXITSTATUS(wstatus) != 0) {
      fprintf(stderr, "Error: subprocess1 returned a non-zero exit code\n"); 
      prob_checker = 1; 
    }

    int w2status; //wait for child process completion
    actual_pid = waitpid(pid2, &w2status, 0); 
    if (actual_pid == -1) {
      fprintf(stderr, "Error: waitpid2 failed!\n"); 
      prob_checker = 1; 
    }

    if (!WIFEXITED(w2status)) {
      fprintf(stderr, "Error: subprocess2 crashed, was interrupted, or did not exit normally\n");
      prob_checker = 1; 
    }
    if (WEXITSTATUS(w2status) != 0) {
      fprintf(stderr, "Error: subprocess2 returned a non-zero exit code\n"); 
      prob_checker = 1; 
    }
  
  }

  if (prob_checker) { //check that no errors occured in child processes
    fprintf(stderr, "Error: child processes did not proceed properly\n"); 
    exit(-1); 
  }
  
  
  int64_t * temparr = (int64_t *) malloc(2 * length * sizeof(int64_t)); //create temparr to hold sorted and merged array

  merge(arr, begin, mid, end, temparr); //merge to temparr
  
  //deep copy
  for(int i = 0; i < length; ++i){ 
    arr[i+begin] = temparr[i];
  }

  free(temparr); //free dynamically-allocated value
  
  return; 

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
    fprintf(stderr, "Error: threshold value is invalid\n");
    return 1; 
  }


  //open the file
  int fd = open(filename, O_RDWR);
  if (fd < 0) {
    fprintf(stderr, "Error: could not open file %s\n", filename);
    return 2; 
  }


  //use fstat to determine the size of the file
  struct stat statbuf;
  int rc = fstat(fd, &statbuf);
  if (rc != 0) {
    fprintf(stderr, "Error: fsat could not properly get information\n");
    return 3; 
  }
  size_t file_size_in_bytes = statbuf.st_size;


  //map the file into memory using mmap
  int64_t *data = mmap(NULL, file_size_in_bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (data == MAP_FAILED) {
    fprintf(stderr, "Error: mmap faled to create new mapping\n");
    return 4; 
  }


  //sort the data!
  merge_sort(data, 0, file_size_in_bytes / sizeof(int64_t), threshold); //here


  //unmap and close the file
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
  
  
  return 0;
}
