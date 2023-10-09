#include <cmath>
#include <iostream>
#include <string.h>
#include <string>
#include "cache.h"

bool check_pow2(int x, int lim) {
  if (x < lim) return false;
  return (std::ceil(log2(x)) == std::floor(log2(x)));
}

int main(int argc, char** argv) {
  if (argc != 7) {
    std::cerr << "Invalid number of arugments" << std::endl;
    return 1;
  }

  int sets_in_cache = std::stoi(argv[1]);
  int blocks_per_set = std::stoi(argv[2]);
  int block_size = std::stoi(argv[3]);

  if (!check_pow2(sets_in_cache, 2) ||
      !check_pow2(blocks_per_set, 2) ||
      !check_pow2(block_size, 4)
  ) {
    std::cerr << "Invalid set, block, or block_size value" << std::endl;
    return 1;
  }

  bool is_write_allocate = !(std::strcmp(argv[4], "write-allocate"));
  bool is_write_through = !(std::strcmp(argv[5], "write-through"));
  bool is_lru = !(std::strcmp(argv[6], "lru"));

  if (!is_write_allocate && !is_write_through) {
    std::cerr << "Cannot use no-write-allocate and write-back together" << std::endl;
    return 1;
  }

  return 0;

}
