#include <cmath>
#include <iostream>
#include <string.h>
#include <string>
#include "cache.h"

bool check_pow2(int x, int lim) {
  if (x < lim) return false;
    return (x > 0) && ((x & (x - 1)) == 0);
}

int main(int argc, char** argv) {
  if (argc != 7) {
    std::cerr << "Invalid number of arugments" << std::endl;
    return 1;
  }

  int sets_in_cache = std::stoi(argv[1]);
  int blocks_per_set = std::stoi(argv[2]);
  int block_size = std::stoi(argv[3]);

  if ((!check_pow2(sets_in_cache, 1) ||
      !check_pow2(blocks_per_set, 1) ||
      !check_pow2(block_size, 4))
  ) {
    std::cerr << "Invalid set, block, or block_size value" << std::endl;
    return 1;
  }

  bool is_write_allocate = !strcmp(argv[4], "write-allocate");
  bool is_write_back = !(strcmp(argv[5], "write-back"));
  bool is_lru = !(strcmp(argv[6], "lru"));

  if (!is_write_allocate && is_write_back) {
    std::cerr << "Cannot use no-write-allocate and write-back together" << std::endl;
    return 1;
  }

  Cache cache(sets_in_cache, blocks_per_set, block_size, is_write_allocate, is_write_back, is_lru);

  int time = 0;
  int offset = log2(block_size);
  int index_bits = log2(sets_in_cache);
  uint32_t address, tag, index;
  std::string trace; 

  while (std::getline(std::cin, trace)) {
    char action = trace.at(0);
    address = (uint32_t)std::stoul(trace.substr(2, 10), NULL, 16);
    index = (address >> offset) & (sets_in_cache - 1);
    tag = address >> (index_bits + offset);
    if (action == 'l') cache.read(index, tag, time);
    else cache.write(index, tag, time);
    ++time;
  }

  cache.print_stats();
  return 0;
}
