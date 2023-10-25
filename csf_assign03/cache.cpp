#include "cache.h"
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <string>

// Method to handle write operations
void Cache::write(uint32_t index, uint32_t tag, int current_time) {
  auto block = sets[index].find(tag);
  if (block != sets[index].end()) {
    ++store_hits;
    cycles += is_write_back ? 1 : 100;
    block->second.access_ts = current_time;
    block->second.is_dirty = is_write_back;
    block->second.access_ts = current_time;
  }
  else {
    ++store_misses;
    cycles += (is_write_allocate ? 100 : 0) + (is_write_back ? 0 : 25 * block_size);
    if (is_write_allocate) {
      handle_write_action(index, tag, current_time); 
      return;
    }
  }
    
}

// Method to handle read operations
void Cache::read(uint32_t index, uint32_t tag, int current_time) {
  auto block = sets[index].find(tag);
  if (block != sets[index].end()) {
    ++load_hits;
    ++cycles;
    block->second.access_ts = current_time;
  }
  else {
    ++load_misses;
    cycles += 25 * block_size + 1;
    if (is_write_allocate) {
      handle_write_action(index, tag, current_time); 
      return;
    }
  }
}

//handle writing to cache depending on implementation
void Cache::handle_write_action(uint32_t index, uint32_t tag, int current_time) {
  if (is_lru) {
    if ((int) sets[index].size() < blocks_per_set) {
      sets[index][tag] = {false, current_time, current_time};
    } else {
      uint32_t lru_tag = tag;
      int oldest_time = current_time;
      for (const auto& block : sets[index]) {
        if (block.second.access_ts < oldest_time) {
          oldest_time = block.second.access_ts;
          lru_tag = block.first;
        }
      }

      sets[index].erase(lru_tag);

      sets[index][tag] = {false, current_time, current_time};
    }
  } else {
    //no FIFO implementation for MS2
    return;
  }
}

void Cache::print_stats() {
    std::cout << "Total loads: " << this->load_hits + this->load_misses << std::endl;
    std::cout << "Total stores: " << this->store_misses + this->store_hits << std::endl;
    std::cout << "Load hits: " << this->load_hits << std::endl;
    std::cout << "Load misses: " << this->load_misses << std::endl;
    std::cout << "Store hits: " << this->store_hits << std::endl;
    std::cout << "Store misses: " << this->store_misses << std::endl;
    std::cout << "Total cycles: " << this->cycles << std::endl;
}
