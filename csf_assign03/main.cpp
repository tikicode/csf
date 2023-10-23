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

  bool is_write_allocate = strcmp(argv[4], "write-allocate");
  bool is_write_back = !(strcmp(argv[5], "write-through"));
  bool is_lru = !(strcmp(argv[6], "lru"));

  if (!is_write_allocate && is_write_back) {
    std::cerr << "Cannot use no-write-allocate and write-back together" << std::endl;
    return 1;
  }

  Cache cache(sets_in_cache, blocks_per_set, block_size, is_write_allocate, is_write_back, is_lru);

  int time = 0;
  int offset = log2(block_size);
  int n_sets = log2(sets_in_cache);
  char action;
  uint32_t address, tag, index;
  std::string trace; 
  while (std::getline(std::cin, trace)) {
    char action = trace.at(0);
    address = (uint32_t)std::stoul(trace.substr(2, 10), NULL, 16);
    tag = address >> (index + offset);
    // extract the bits of the index using a binary mask (0 if 0, 1... if mult of 2)
    index = (address >> offset) & (n_sets - 1);
    if (action == 'l') cache.read(index, tag, time);
    else cache.write(index, tag, time);
    ++time;
  }
  return 0;
}
