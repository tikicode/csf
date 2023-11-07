#include <stddef.h>
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

int compare_i64(const void *left_, const void *right_) {
  int64_t left = *(int64_t *)left_;
  int64_t right = *(int64_t *)right_;
  if (left < right) return -1;
  if (left > right) return 1;
  return 0;
}

void seq_sort(int64_t *arr, size_t begin, size_t end) {
  size_t num_elements = end - begin;
  qsort(arr + begin, num_elements, sizeof(int64_t), compare_i64);
}

// Merge the elements in the sorted ranges [begin, mid) and [mid, end),
// copying the result into temparr.
void merge(int64_t *arr, size_t begin, size_t mid, size_t end, int64_t *temparr) {
  int64_t *endl = arr + mid;
  int64_t *endr = arr + end;
  int64_t *left = arr + begin, *right = arr + mid, *dst = temparr;

  for (;;) {
    int at_end_l = left >= endl;
    int at_end_r = right >= endr;

    if (at_end_l && at_end_r) break;

    if (at_end_l)
      *dst++ = *right++;
    else if (at_end_r)
      *dst++ = *left++;
    else {
      int cmp = compare_i64(left, right);
      if (cmp <= 0)
        *dst++ = *left++;
      else
        *dst++ = *right++;
    }
  }
}

void fatal(const char *msg) __attribute__ ((noreturn));

void fatal(const char *msg) {
  fprintf(stderr, "Error: %s\n", msg);
  exit(1);
}

void merge_sort(int64_t *arr, size_t begin, size_t end, size_t threshold) {
  assert(end >= begin);
  size_t size = end - begin;

  if (size <= threshold) {
    seq_sort(arr, begin, end);
    return;
  }

  // recursively sort halves in parallel

  size_t mid = begin + size/2;

  // TODO: parallelize the recursive sorting
  merge_sort(arr, begin, mid, threshold);
  merge_sort(arr, mid, end, threshold);

  // success!
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
    // TODO: report an error (threshold value is invalid)
    fprintf(stderr, "threshold value is invalid");
    return -1;
  }

  // TODO: open the file
  int fd = open(filename, O_RDWR);
  if (fd < 0) {
    fprintf(stderr, "Invalid input file");
    return 2;
  }

  // TODO: use fstat to determine the size of the file
  struct stat buffer;
  if (fstat(fd, &buffer) != 0) {
    fprintf(stderr, "Error getting file statistics");
    return 2;
  }


  size_t file_size_bytes = buffer.st_size;
  size_t num_elements = file_size_bytes/8;
  // TODO: map the file into memory using mmap

  int64_t *data = mmap(NULL, file_size_bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  close(fd);

  if (data == MAP_FAILED) {
    fprintf(stderr, "could not map file");
    return 3;
  }

  // TODO: sort the data!
    // Sort the data if below threshold or else fork child processes
  if (num_elements <= threshold) {
    merge_sort(data, 0, num_elements, threshold);
  } else {
    pid_t pid_l, pid_r;
    int status_l, status_r;

    // Sort left half in child process
    pid_l = fork();
    if (pid_l == 0) {
      merge_sort(data, 0, num_elements / 2, threshold);
      exit(0); // Child exits after sorting
    } else if (pid_l < 0) {
      fatal("Failed to fork left child");
    }

    // Sort right half in child process
    pid_r = fork();
    if (pid_r == 0) {
      merge_sort(data, num_elements / 2, num_elements, threshold);
      exit(0); // Child exits after sorting
    } else if (pid_r < 0) {
      fatal("Failed to fork right child");
    }

    // Wait for both child processes to finish
    waitpid(pid_l, &status_l, 0);
    if (WIFEXITED(status_l) == 0 || WEXITSTATUS(status_l) != 0) {
      fatal("Left child did not terminate correctly");
    }

    waitpid(pid_r, &status_r, 0);
    if (WIFEXITED(status_r) == 0 || WEXITSTATUS(status_r) != 0) {
      fatal("Right child did not terminate correctly");
    }

    // Now that both children are done, merge the two sorted halves
    int64_t *temp_arr = (int64_t *)malloc(num_elements * sizeof(int64_t));
    if (temp_arr == NULL) {
      fatal("malloc() failed");
    }

    merge(data, 0, num_elements / 2, num_elements, temp_arr);

    // Copy data back to main array
    for (size_t i = 0; i < num_elements; i++) {
      data[i] = temp_arr[i];
    }

    // Free the temporary array
    free(temp_arr);
  }
  
  // TODO: unmap and close the file

  munmap(data, file_size_bytes);

  // TODO: exit with a 0 exit code if sort was successful
  
  return 0;
}
