#include <cmath>
#include <cstring>
#include <iostream>
#include <string>

#include "cache.h"

/*
 * x: int -> int value that we are chhecking
 * minimum -> the minimum value that x can be
 */
bool is_power_of_two(int x, int minimum) {
  return x >= minimum && (x > 0) && ((x & (x - 1)) == 0);
}

/*
 * Function to ensure input arguments are valid
 * argc : int -> number of input args
 * argv : char** -> list of command line inputs
 * sets : int& -> number of sets in cache
 * blocks : int& -> number of blocks in cache
 * block_size : int& -> block size in bytes
 * write_allocate : bool& -> write allocate setting
 * write_back : bool& -> write back setting
 * lru : bool& -> is lru cache (else fifo)
 */
bool validate_arguments(int argc, char** argv, int& sets, int& blocks,
                        int& block_size, bool& write_allocate, bool& write_back,
                        bool& lru) {
  // check for correct number of arguments
  if (argc != 7) {
    std::cerr << "Invalid number of arguments" << std::endl;
    return false;
  }
  // store the input values of number of sets, number of blocks and block size
  // to corresponding variables
  sets = std::stoi(argv[1]);
  blocks = std::stoi(argv[2]);
  block_size = std::stoi(argv[3]);

  // check that input numbers are valid
  if (!is_power_of_two(sets, 1) || !is_power_of_two(blocks, 1) ||
      !is_power_of_two(block_size, 4)) {
    std::cerr << "Invalid set, block, or block_size value" << std::endl;
    return false;
  }

  // setting write_allocate and write_back booleans based on input and deciding
  // what eviction method to use based on input
  write_allocate = !strcmp(argv[4], "write-allocate");
  write_back = !(strcmp(argv[5], "write-back"));
  lru = !(strcmp(argv[6], "lru"));

  // cannot be non_write_allocate and write_back
  if (!write_allocate && write_back) {
    std::cerr << "Cannot use no-write-allocate and write-back together"
              << std::endl;
    return false;
  }
  return true;
}

/*
 * Stack trace parser
 * cache : Cache& -> cache simulator object
 * sets : int -> number of sets in cache (pow of 2)
 * block_size : int -> number of bytes in a block (pow of 2)
 */
void process_trace(Cache& cache, int sets, int block_size) {
  // setting variables to calculate index and the time variable
  int time = 0;
  int offset = log2(block_size);
  int indexBits = log2(sets);
  uint32_t address, tag, index;
  std::string trace;

  // parsing through input file
  while (std::getline(std::cin, trace)) {
    char action = trace.at(0);
    // calculating the address
    address = (uint32_t)std::stoul(trace.substr(2, 10), NULL, 16);
    // calculating index (set that the data is in)
    index = (address >> offset) & (sets - 1);
    // calculating the tag
    tag = address >> (indexBits + offset);
    // if file specifies l, it is load and we call our read function, else it
    // specifies s for store, so we call our write
    if (action == 'l')
      cache.read(index, tag, time);
    else
      cache.write(index, tag, time);
    // increment the time
    ++time;
  }

  // calling the method to display the stats that we found
  cache.print_stats();
}
/*
 * argc -> int: number of arguments
 * argv -> arg list
 */
int main(int argc, char** argv) {
  int sets_in_cache, blocks_per_set, block_size;
  bool is_write_allocate, is_write_back, is_lru;
  if (!validate_arguments(argc, argv, sets_in_cache, blocks_per_set, block_size,
                          is_write_allocate, is_write_back, is_lru)) {
    return 1;
  }
  Cache cache(sets_in_cache, blocks_per_set, block_size, is_write_allocate,
              is_write_back, is_lru);
  process_trace(cache, sets_in_cache, block_size);

  return 0;
}
