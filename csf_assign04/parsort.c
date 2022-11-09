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
  const int64_t *A = a, *B = b;
  return (*A > *B) - (*A < *B);//truncation, this doesn't work (don't cast to int*, just int64*);
}

void merge(int64_t *arr, size_t begin, size_t mid, size_t end, int64_t *temparr) { //segfault
  // don't use inclusive endpoints, you wouldn't have to do mid + 1
  size_t counter = 0;
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
    temparr[counter] = arr[begin]; //segfault
    begin++;
    counter++;
  }

  while(midplus <= end){
    temparr[counter] = arr[midplus];
    midplus++;
    counter++;
  }
}

int merge_sort(int64_t *arr, size_t begin, size_t end, size_t threshold) {
  //check for sorted 
  int mid = (begin + end) / 2; 

  int64_t length =(end + 1) - begin;
  
  int64_t temparr = (int64_t) malloc(length * sizeof(int64_t)); 
  
  if((end-begin+1) <= threshold){
    qsort(arr, end-begin, sizeof(int64_t), cmpfunc);
    return 0;
  } else{
    pid_t pid = fork(); 
    if (pid == -1) {
      fprintf(stderr, "Error: fork failed!\n");
      return -1; 
    } else if (pid == 0) {
      int return_val = merge_sort(arr, begin, mid, threshold); 
      exit(return_val);
    } 
    
    pid_t pid2 = fork(); 
    if (pid2 == -1) {
      fprintf(stderr, "Error: fork failed!\n"); 
      return -1;
    } else if (pid2 == 0) {
      int sreturn_val = merge_sort(arr, mid + 1, end, threshold); 
      exit(sreturn_val); 
    }

    int wstatus; 
    pid_t actual_pid = waitpid(pid, &wstatus, 0); 
    if (actual_pid == -1) {
      fprintf(stderr, "Error: waitpid1 failed!\n"); 
      return -1; 
    }

    if (!WIFEXITED(wstatus)) {
      fprintf(stderr, "Error: subprocess1 crashed, was interrupted, or did not exit normally\n");
      return -1; 
    }
    if (WEXITSTATUS(wstatus) != 0) {
      fprintf(stderr, "Error: subprocess1 returned a non-zero exit code\n"); 
      return -1; 
    }

    actual_pid = waitpid(pid2, &wstatus, 0); 
    if (actual_pid == -1) {
      fprintf(stderr, "Error: waitpid2 failed!\n"); 
      return -1; 
    }

    if (!WIFEXITED(wstatus)) {
      fprintf(stderr, "Error: subprocess2 crashed, was interrupted, or did not exit normally\n");
      return -1; 
    }
    if (WEXITSTATUS(wstatus) != 0) {
      fprintf(stderr, "Error: subprocess2 returned a non-zero exit code"); 
      return -1; 
    }
    
    /**
    else {

      int wstatus; 
      pid_t actual_pid = waitpid(pid, &wstatus, 0); 
      if (actual_pid == -1) {
        fprintf(stderr, "Error: waitpid failed!\n"); 
        return -1; 
      }

      if (!WIFEXITED(wstatus)) {
        fprintf(stderr, "Error: subprocess crashed, was interrupted, or did not exit normally\n");
        return -1; 
      }
      if (WEXITSTATUS(wstatus) != 0) {
        fprintf(stderr, "Error: subprocess returned a non-zero exit code"); 
        return -1; 
      }

      merge_sort(arr, mid + 1, end, threshold); 

    }
    **/ 
  }

  merge(arr, begin, mid, end, &temparr); 
  arr = &temparr; 
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
  merge_sort(data, 0, file_size_in_bytes-1, threshold);


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
