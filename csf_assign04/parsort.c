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
    pid_t pid_l = fork();
    if (pid_l == -1) {
      fprintf(stderr, "failed to load process");
    } else if (pid_l == 0) {
      merge_sort(arr, begin, mid, threshold);
      exit(0);
    }

    pid_t pid_r = fork();
    if (pid_r == -1) {
      fprintf(stderr, "failed to load process");
    } else if (pid_r == 0) {
      merge_sort(arr, mid, end, threshold);
      exit(0);
    }

    int left_wstatus;
    // blocks until the process indentified by pid_to_wait_for completes
    pid_t left_pid = waitpid(pid_l, &left_wstatus, 0);
    if (left_pid == -1) {
      fprintf(stderr, "child process failure");
    }

    int right_wstatus;
    // blocks until the process indentified by pid_to_wait_for completes
    pid_t right_pid = waitpid(pid_r, &right_wstatus, 0);
    if (right_pid == -1) {
      fprintf(stderr, "child process failure");
    }

  // allocate temp array now, so we can avoid unnecessary work
  // if the malloc fails
  int64_t *temp_arr = (int64_t *) malloc(size * sizeof(int64_t));
  if (temp_arr == NULL)
    fatal("malloc() failed");

  // child processes completed successfully, so in theory
  // we should be able to merge their results
  merge(arr, begin, mid, end, temp_arr);

  // copy data back to main array
  for (size_t i = 0; i < size; i++)
    arr[begin + i] = temp_arr[i];

  // now we can free the temp array
  free(temp_arr);

  // success!
}

int handle_child_work(size_t is_left, int64_t *arr, size_t begin, size_t end, size_t threshold) {
  size_t size = end - begin;
  size_t mid = begin + size/2;
  if(is_left) {
    merge_sort(arr, begin, mid, threshold);
  } else {
    merge_sort(arr, mid, end, threshold);
  }

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
    merge_sort(data, 0, num_elements, threshold);


  // TODO: unmap and close the file

  munmap(data, num_elements);

  // TODO: exit with a 0 exit code if sort was successful
  return 0;
}
