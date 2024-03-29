#include <assert.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

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
void merge(int64_t *arr, size_t begin, size_t mid, size_t end,
           int64_t *temparr) {
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

void fatal(const char *msg) __attribute__((noreturn));

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
  size_t mid = begin + size / 2;

  // parallelize the recursive sorting
  pid_t pid_l, pid_r;
  int status_l, status_r;

  // Sort left half in child process
  pid_l = fork();
  if (pid_l == -1) {
    fatal("Failed to fork left child");
  } else if (!pid_l) {
    merge_sort(arr, begin, mid, threshold);
    exit(0);  // Child exits after sorting
  }

  // Sort right half in child process
  pid_r = fork();
  if (pid_r == -1) {
    fatal("Failed to fork right child");
  } else if (!pid_r) {
    merge_sort(arr, mid, end, threshold);
    exit(0);  // Child exits after sorting
  }

  // Wait for both child processes to finish and handle
  pid_t l_actual_pid = waitpid(pid_l, &status_l, 0);
  if (l_actual_pid == -1) {
    fatal("Failed to wait for left child");
  }
  if (!WIFEXITED(status_l) || WEXITSTATUS(status_l)) {
    fatal("Left child did not terminate correctly");
    exit(1);
  }

  pid_t r_actual_pid = waitpid(pid_r, &status_r, 0);
  if (r_actual_pid == -1) {
    fatal("Failed to wait for right child");
  }
  if (!WIFEXITED(status_r) || WEXITSTATUS(status_r)) {
    fatal("Right child did not terminate correctly");
    exit(1);
  }

  // Now that both children are done, merge the two sorted halves
  // Allocate temporary array for merging
  int64_t *temp_arr = (int64_t *)malloc(size * sizeof(int64_t));
  merge(arr, begin, mid, end, temp_arr);

  for (size_t i = 0; i < size; i++) arr[begin + i] = temp_arr[i];

  free(temp_arr);
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
  size_t threshold = (size_t)strtoul(argv[2], &end, 10);
  if (end != argv[2] + strlen(argv[2])) {
    // report an error (threshold value is invalid)
    fatal("threshold value is invalid");
  }

  // open the file
  int fd = open(filename, O_RDWR);
  if (fd < 0) {
    fatal("threshold value is invalid");
    close(fd);
    return 1;
  }

  // use fstat to determine the size of the file
  struct stat buffer;
  if (fstat(fd, &buffer) != 0) {
    fatal("Error getting file statistics");
  }
  size_t file_size_bytes = buffer.st_size;
  size_t num_elements = file_size_bytes / 8;

  // map the file into memory using mmap
  int64_t *data =
      mmap(NULL, file_size_bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  close(fd);

  if (data == MAP_FAILED) {
    fatal("could not map file");
  }

  // sort the data!
  merge_sort(data, 0, num_elements, threshold);

  // unmap
  munmap(data, file_size_bytes);

  // exit with a 0 exit code if sort was successful
  return 0;
}
