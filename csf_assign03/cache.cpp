#include "cache.h"
#include <iostream>
#include <unordered_map>
#include <string>

// Method to handle write operations
void Cache::write(uint32_t index, uint32_t tag, int current_time) {
  auto block = sets[index].find(tag);
  if (block != sets[index].end()) {
    store_hits++;
    cycles += is_write_back ? 1 : 100;
    block->second.access_ts = current_time;
    block->second.is_dirty = is_write_back;
  }
  else {
    cycles += (is_write_allocate ? 100 : 0) + (is_write_back ? 0 : 25 * block_size);
    store_misses++;
    if (is_write_allocate) {
      // add new block to cache 
      return;
    }
  }
    
}

// Method to handle read operations
void Cache::read(uint32_t index, uint32_t tag, int current_time) {
  auto block = sets[index].find(tag);
  if (block != sets[index].end()) {
    load_hits++;
    cycles++;
    block->second.access_ts = current_time;
  }
  else {
    cycles += 25 * block_size + 1;
    load_misses++;
    if (is_write_allocate) {
    // add new block to cache 
      return;
    }
  }
}

void Cache::print_stats() {
    std::cout << "Total loads: " << this->load_hits + this->load_misses << std::endl;
    std::cout << "Total stores" << this->store_misses + this->store_hits << std::endl;
    std::cout << "Load hits: " << this->load_hits << std::endl;
    std::cout << "Load misses: " << this->load_misses << std::endl;
    std::cout << "Store hits:" << this->store_hits << std::endl;
    std::cout << "Store misses" << this->store_misses << std::endl;
    std::cout << "Total cycles" << this->cycles << std::endl;
}
