#include <cmath>
#include <cstring>
#include <iostream>
#include <string>
#include "cache.h"

bool isPowerOfTwo(int x, int minimum) {
  return x >= minimum && (x > 0) && ((x & (x - 1)) == 0);
}

bool validateArguments(int argc, char** argv, int& sets, int& blocks, int& blockSize, bool& writeAllocate, bool& writeBack, bool& lru) {
  if (argc != 7) {
    std::cerr << "Invalid number of arguments" << std::endl;
    return false;
  }

  sets = std::stoi(argv[1]);
  blocks = std::stoi(argv[2]);
  blockSize = std::stoi(argv[3]);

  if (!isPowerOfTwo(sets, 1) || !isPowerOfTwo(blocks, 1) || !isPowerOfTwo(blockSize, 4)) {
    std::cerr << "Invalid set, block, or block_size value" << std::endl;
    return false;
  }

  writeAllocate = !strcmp(argv[4], "write-allocate");
  writeBack = !(strcmp(argv[5], "write-back"));
  lru = !(strcmp(argv[6], "lru"));

  if (!writeAllocate && writeBack) {
    std::cerr << "Cannot use no-write-allocate and write-back together" << std::endl;
    return false;
  }

  return true;
}

void processTrace(Cache& cache, int sets, int blockSize) {
  int time = 0;
  int offset = log2(blockSize);
  int indexBits = log2(sets);
  uint32_t address, tag, index;
  std::string trace;

  while (std::getline(std::cin, trace)) {
    char action = trace.at(0);
    address = (uint32_t)std::stoul(trace.substr(2, 10), NULL, 16);
    index = (address >> offset) & (sets - 1);
    tag = address >> (indexBits + offset);
    if (action == 'l') cache.read(index, tag, time);
    else cache.write(index, tag, time);
    ++time;
  }

  cache.print_stats();
}

int main(int argc, char** argv) {
  int sets_in_cache, blocks_per_set, block_size;
  bool is_write_allocate, is_write_back, is_lru;

  if (!validateArguments(argc, argv, sets_in_cache, blocks_per_set, block_size, is_write_allocate, is_write_back, is_lru)) {
    return 1;
  }

  Cache cache(sets_in_cache, blocks_per_set, block_size, is_write_allocate, is_write_back, is_lru);
  processTrace(cache, sets_in_cache, block_size);

  return 0;
}
