#ifndef CACHE_H
#define CACHE_H

#include <iostream>
#include <unordered_map>
#include <iostream>

struct Stored { bool is_dirty; int created, accessed; };

class Cache{
  private:
    int sets_in_cache, blocks_per_set, block_size;
    bool is_write_through, is_write_allocate, is_lru;
    int load_hits = 0, load_misses = 0, store_hits = 0;
    int store_misses = 0, cycles = 0;
    
    // make var for sets 

  public:
    Cache(
      int sets_in_cache,
      int blocks_per_set,
      int block_size,
      bool is_write_allocate,
      bool is_write_through,
      bool is_lru
    ) {
      this->sets_in_cache = sets_in_cache;
      this->blocks_per_set = blocks_per_set;
      this->block_size = block_size;
      this->is_write_allocate = is_write_allocate;
      this->is_write_through = is_write_through;
      this->is_lru = is_lru;
    };

};

#endif
