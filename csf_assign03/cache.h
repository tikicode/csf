#ifndef CACHE_H
#define CACHE_H

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>

struct Slots {
  bool is_dirty;
  int insert_ts, access_ts;
};
typedef std::unordered_map<uint32_t, Slots> row;

/*
 * Cache Simulator
 */
class Cache {
 private:
  std::vector<row> sets;
  std::queue<uint32_t> fifo;
  int sets_in_cache, blocks_per_set, block_size;
  bool is_write_back, is_write_allocate, is_lru;
  long int load_hits = 0, load_misses = 0, store_hits = 0, store_misses = 0,
           cycles = 0;

 public:
  Cache(int sets_in_cache, int blocks_per_set, int block_size,
        bool is_write_allocate, bool is_write_back, bool is_lru) {
    this->sets_in_cache = sets_in_cache;
    this->blocks_per_set = blocks_per_set;
    this->block_size = block_size;
    this->is_write_allocate = is_write_allocate;
    this->is_write_back = is_write_back;
    this->is_lru = is_lru;

    for (int i = 0; i < sets_in_cache; ++i) {
      sets.push_back(row());
    }
  };

  /*
   * Cache read function
   * index : uint32_t -> set index of the memory address
   * tag : uint32_t -> tag for address within a block
   * current_time : int -> time of cache read
   */
  void read(uint32_t index, uint32_t tag, int current_time);

  /*
   * Cache write function
   * index : uint32_t -> set index of the memory address
   * tag : uint32_t -> tag for address within a block
   * current_time : int -> time of cache write
   */
  void write(uint32_t index, uint32_t tag, int current_time);

  /*
   * Cache modification function
   * index : uint32_t -> set index of the memory address
   * tag : uint32_t -> tag for address within a block
   * current_time : int -> time of write to cache
   */
  void handle_write_action(uint32_t index, uint32_t tag, int current_time);

  /*
   * Cache information print helper
   */
  void print_stats();
};

#endif
