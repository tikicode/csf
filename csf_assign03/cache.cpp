#include "cache.h"

#include <cstdint>
#include <iostream>
#include <limits>
#include <string>
#include <unordered_map>

void Cache::write(uint32_t index, uint32_t tag, int current_time) {
  auto block = sets[index].find(tag);
  if (block != sets[index].end()) {
    ++store_hits;
    if (is_write_back)
      cycles += 1;
    else
      cycles += 101;  // write tag (4 byte) to cache if write through
    block->second.access_ts = current_time;
    block->second.is_dirty = is_write_back;
  } else {
    ++store_misses;
    if (!is_write_back) cycles += 101;
    if (is_write_allocate) {
      cycles += 25 * block_size + 1;  // write block to memory
      handle_write_action(index, tag, current_time);
    }
  }
}

void Cache::read(uint32_t index, uint32_t tag, int current_time) {
  auto block = sets[index].find(tag);
  if (block != sets[index].end()) {
    ++load_hits;
    ++cycles;
    block->second.access_ts = current_time;
  } else {
    ++load_misses;
    cycles += 25 * block_size + 1;  // fetch address from memory if miss
    handle_write_action(index, tag, current_time);
    return;
  }
}

void Cache::handle_write_action(uint32_t index, uint32_t tag,
                                int current_time) {
  if ((int)sets[index].size() < blocks_per_set) {
    Slots slot = {false, current_time, current_time};
    sets[index][tag] = slot;
    return;
  }
  uint32_t del_tag;
  int oldest_time = current_time;
  int cur_time = current_time;
  for (const auto& block : sets[index]) {
    if (is_lru) cur_time = block.second.access_ts;
    else cur_time = block.second.insert_ts;
    if (cur_time < oldest_time) {
      if (is_lru) oldest_time = block.second.access_ts;  // find oldest cached tag
      else oldest_time = block.second.insert_ts; 
      del_tag = block.first;
    }
  }
  sets[index].erase(del_tag);
  if (is_write_back)
    cycles += 25 * block_size + 1;  // write evicted block to memory
  Slots slot = {false, current_time, current_time};
  sets[index][tag] = slot;
}

void Cache::print_stats() {
  std::cout << "Total loads: " << this->load_hits + this->load_misses
            << std::endl;
  std::cout << "Total stores: " << this->store_misses + this->store_hits
            << std::endl;
  std::cout << "Load hits: " << this->load_hits << std::endl;
  std::cout << "Load misses: " << this->load_misses << std::endl;
  std::cout << "Store hits: " << this->store_hits << std::endl;
  std::cout << "Store misses: " << this->store_misses << std::endl;
  std::cout << "Total cycles: " << this->cycles << std::endl;
}
