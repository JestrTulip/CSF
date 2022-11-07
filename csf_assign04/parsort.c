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

void merge(int64_t *arr, size_t begin, size_t mid, size_t end, int64_t *temparr) {
  size_t i = begin; 
  size_t j = mid; 

  for (size_t k = begin; k < end; k++) {
    if (i < mid && (j>=end || arr[i] <= arr[j])) { 
      temparr[k] = arr[i]; 
      i++; 
    } else { 
      temparr[k] = arr[j];
      j++;
    }
  }
}

void merge_sort(int64_t *arr, size_t begin, size_t end, size_t threshold) {
  // TODO: implement
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

  //open the file
  int fd = open(filename, O_RDWR); 
  if (fd < 0) {
    fprintf(stderr, "Error: file could not be opened\n"); 
    return 2; 
  }

  //use fstat to determine the size of the file
  struct stat statbuf; 
  int rc = fstat(fd, &statbuf);
  if (rc != 0) {
    fprintf(stderr, "Error: fstat failed to properly obtain file status info\n");
    return 3; 
  }

  // TODO: map the file into memory using mmap
  size_t byte_file_size = statbuf.st_size; 
  int64_t *data = mmap(NULL, byte_file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); 
  if (data == MAP_FAILED) { 
    fprintf(stderr, "Error: mmap failed to created shared read-write mapping\n");
    return 4; 
  }

  // TODO: sort the data!
  

  // TODO: unmap and close the file
  munmap(data, byte_file_size);
  close(fd); 
  // TODO: exit with a 0 exit code if sort was successful

}
