#ifndef CACHE_H
#define CACHE_H

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <iostream>
#include <vector>

struct Slots { bool is_dirty; int insert_ts, access_ts; };
typedef std::unordered_map<uint32_t, Slots> row;

class Cache{
  private:
    std::vector<row> sets;

    int sets_in_cache, blocks_per_set, block_size;
    bool is_write_back, is_write_allocate, is_lru;
    int load_hits, load_misses, store_hits, store_misses, cycles;

  public:
    Cache(
      int sets_in_cache,
      int blocks_per_set,
      int block_size,
      bool is_write_allocate,
      bool is_write_back,
      bool is_lru
    ) {
      this->sets_in_cache = sets_in_cache;
      this->blocks_per_set = blocks_per_set;
      this->block_size = block_size;
      this->is_write_allocate = is_write_allocate;
      this->is_write_back = is_write_back;
      this->is_lru = is_lru;
    };

    void read(uint32_t index, uint32_t tag, int current_time);

    void write(uint32_t index, uint32_t tag, int current_time);

    void handle_write_action(uint32_t index, uint32_t tag, int current_time);

    void print_stats();
};

#endif
