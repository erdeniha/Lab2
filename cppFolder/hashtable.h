#pragma once
#include <string>
#include <cstdint>
#include "list.h"

enum ProbeMode { PROBE_LINEAR, PROBE_DOUBLE };
enum SlotState { SLOT_EMPTY, SLOT_USED, SLOT_DELETED };
enum InsertResult { HT_ADDED, HT_UPDATED, HT_FULL };

struct Slot {
    std::string key;
    std::string value;
    DNode* node;      
    SlotState state;
};

struct HashTable {
    Slot* slots;
    int capacity;      
    int used;          
    int deleted;       // могила
    ProbeMode mode;
    bool autoGrow;     // расширять таблицу при заполнении больше 70 проце
    long long probes;  // сколько всего слотов просмотрено для эмпирического
};

uint64_t foldKey(const std::string& key, int groupSize);
std::string foldParts(const std::string& key, int groupSize); 
int nextPrime(int n);
void initHashTable(HashTable* table, ProbeMode mode, int capacity, bool autoGrow);
void freeHashTable(HashTable* table);
InsertResult htInsert(HashTable* table, const std::string& key, const std::string& value, DNode* node);
Slot* htFind(HashTable* table, const std::string& key);
bool htErase(HashTable* table, const std::string& key);
int htSize(const HashTable* table);
void printHashTable(const HashTable* table);
