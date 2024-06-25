#include <unordered_map>
#include <vector>
#include <cstring>
#include "map_std.h"
#include "map.h"

extern "C" {

typedef struct {
    std::unordered_map<std::string, std::vector<uint8_t>> map;
    map_counters_t counters;
    size_t key_size;
    size_t val_size;
} std_map_t;

void* new_std_map(const size_t size, const size_t key_size, const size_t val_size) {
    std_map_t* wrapper = new std_map_t();
    wrapper->key_size = key_size;
    wrapper->val_size = val_size;
    return wrapper;
}

void free_std_map(void* map) {
    delete static_cast<std_map_t*>(map);
}

uint8_t insert_std(const void* map, const uint8_t* key, const uint8_t* value) {
    std_map_t* wrapper = (std_map_t*)map;
    std::string key_str(reinterpret_cast<const char*>(key), wrapper->key_size);
    std::vector<uint8_t> val_vec(value, value + wrapper->val_size);
    auto result = wrapper->map.insert({key_str, val_vec});
    wrapper->counters.insert++;
    return result.second ? MAP_SUCCESS : MAP_DUPLICATE;
}

uint8_t* find_std(const void* map, const uint8_t* key) {
    std_map_t* wrapper = (std_map_t*)map;
    std::string key_str(reinterpret_cast<const char*>(key), wrapper->key_size);
    auto it = wrapper->map.find(key_str);
    if (it != wrapper->map.end()) {
        return it->second.data();
    }
    return nullptr;
}

uint8_t erase_std(const void* map, const uint8_t* key) {
    std_map_t* wrapper = (std_map_t*)map;
    std::string key_str(reinterpret_cast<const char*>(key), wrapper->key_size);
    auto result = wrapper->map.erase(key_str);
    return result > 0 ? MAP_SUCCESS : MAP_ERROR;
}

size_t count_std(const void* map) {
    std_map_t* wrapper = (std_map_t*)map;
    return wrapper->map.size();
}

void get_stat_std(const void* map, map_counters_t* statistic) {
    std_map_t* wrapper = (std_map_t*)map;
    *statistic = wrapper->counters;
}

} // extern "C"
