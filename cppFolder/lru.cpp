#include "lru.h"

void initLRU(LRUCache* cache, int capacity) {
    cache->capacity = capacity;
    initHashTable(&cache->table, PROBE_DOUBLE, 2 * capacity + 1, true);
    initDList(&cache->order);
}

void freeLRU(LRUCache* cache) {
    freeHashTable(&cache->table);
    freeDList(&cache->order);
}

void lruSet(LRUCache* cache, const std::string& key, const std::string& value) {
    Slot* slot = htFind(&cache->table, key);
    if (slot != nullptr) { 
        slot->value = value;
        dMoveToHead(&cache->order, slot->node);
        return;
    }
    if (htSize(&cache->table) == cache->capacity) {  // кэш полон: выбрасываем самый давний ключ
        std::string oldest;
        dDelTail(&cache->order, oldest);
        htErase(&cache->table, oldest);
    }
    dAddHead(&cache->order, key);
    htInsert(&cache->table, key, value, cache->order.head);
}

bool lruGet(LRUCache* cache, const std::string& key, std::string& value) {
    Slot* slot = htFind(&cache->table, key);
    if (slot == nullptr) {
        return false;
    }
    dMoveToHead(&cache->order, slot->node);
    value = slot->value;
    return true;
}
