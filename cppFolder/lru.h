#pragma once
#include <string>

#include "hashtable.h"
#include "list.h"

struct LRUCache {
    int capacity;
    HashTable table;  
    DList order;      
};

void initLRU(LRUCache* cache, int capacity);  
void freeLRU(LRUCache* cache);
void lruSet(LRUCache* cache, const std::string& key, const std::string& value);
bool lruGet(LRUCache* cache, const std::string& key, std::string& value); 
