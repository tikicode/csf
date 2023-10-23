#include "cache.h"
#include <iostream>
#include <unordered_map>
#include <string>

// Method to handle write operations
void Cache::write(row &set, uint32_t tag, int currentTime) {
    auto it = set.find(tag);

    //checking if address is in cache
    if (it != set.end()) {
        //since we're writing to an aready stored position
        store_hits++;

        it->second.access_ts = currentTime;

        if (is_write_through) {
            it->second.is_dirty = false;
        } else {
            it->second.is_dirty = true;
        } 
    } else {
        store_misses++;

        if (is_write_allocate) {
            if (set.size() >= blocks_per_set) {
                auto lru = set.begin();
                for (auto itr = set.begin(); itr != set.end(); ++itr) {
                    if (itr->second.access_ts < lru->second.access_ts) {
                        lru = itr;
                    }
                }
                set.erase(lru);
            }

            
        }
    }
    


    
}

// Method to handle read operations
void Cache::read(uint32_t address, uint32_t tag, int currentTime) {
    if (this->set.find(address) != this->set.end()) {
        // Update the access time
        this->set[address].access_ts = currentTime;
        load_hits++;
    } else {
        // If it's a miss and the policy is to load into cache on a read miss
        if (this->is_write_allocate) { // Assuming the same policy for read operations
            // Handle the case where the cache set might be full 
            // STILL TODO

            // Add the new address to the cache
            this->set[address] = Slots{false, currentTime, currentTime}; // is_dirty is false because we're reading
        }
        load_misses++;
    }
    cycles++;
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
