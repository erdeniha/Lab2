#include "hashtable.h"
#include <iostream>
#include <cstdint>

static bool allDigits(const std::string& key) {
    if (key.empty()) {
        return false;
    }
    for (size_t i = 0; i < key.size(); i++) {
        if (key[i] < '0' || key[i] > '9') {
            return false;
        }
    }
    return true;
}

uint64_t foldKey(const std::string& key, int groupSize) {
    bool numeric = allDigits(key);
    uint64_t base = numeric ? 10 : 256;
    uint64_t sum = 0;
    uint64_t part = 0;
    int length = 0;
    for (size_t i = 0; i < key.size(); i++) {
        uint64_t digit = numeric ? key[i] - '0' : (unsigned char)key[i];
        part = part * base + digit;
        length++;
        if (length == groupSize) {
            sum += part;
            part = 0;
            length = 0;
        }
    }
    return sum + part;
}

std::string foldParts(const std::string& key, int groupSize) {
    std::string text = "";
    for (size_t i = 0; i < key.size(); i += groupSize) {
        if (i > 0) {
            text += "+";
        }
        text += key.substr(i, groupSize);
    }
    return text;
}

int nextPrime(int n) {
    if (n < 2) {
        n = 2;
    }
    while (true) {
        bool prime = true;
        for (int d = 2; (uint64_t)d * d <= n; d++) {
            if (n % d == 0) {
                prime = false;
                break;
            }
        }
        if (prime) {
            return n;
        }
        n++;
    }
}

void initHashTable(HashTable* table, ProbeMode mode, int capacity, bool autoGrow) {
    if (capacity < 5) {
        capacity = 5;
    }
    table->capacity = nextPrime(capacity);  // шаг двойного хеширования взаимно прост с ним
    table->slots = new Slot[table->capacity];
    for (int i = 0; i < table->capacity; i++) {
        table->slots[i].node = nullptr;
        table->slots[i].state = SLOT_EMPTY;
    }
    table->used = 0;
    table->deleted = 0;
    table->mode = mode;
    table->autoGrow = autoGrow;
    table->probes = 0;
}

void freeHashTable(HashTable* table) {
    delete[] table->slots;
    table->slots = nullptr;
    table->capacity = 0;
    table->used = 0;
    table->deleted = 0;
}

static int locate(HashTable* table, const std::string& key, bool& found) {
    uint64_t start = foldKey(key, 3) % table->capacity;
    uint64_t step = 1; 
    if (table->mode == PROBE_DOUBLE) {
        step = 1 + foldKey(key, 2) % (table->capacity - 1);  // вторая хеш-функция, шаг от 1 до capacity-1
    }

    int firstDeleted = -1;
    found = false;
    for (int i = 0; i < table->capacity; i++) {
        int index = (int)((start + i * step) % table->capacity);
        table->probes++;
        Slot* slot = &table->slots[index];
        if (slot->state == SLOT_EMPTY) {
            return firstDeleted >= 0 ? firstDeleted : index;
        }
        if (slot->state == SLOT_DELETED) {
            if (firstDeleted < 0) {
                firstDeleted = index;
            }
        } else if (slot->key == key) {
            found = true;
            return index;
        }
    }
    return firstDeleted;
}

// перестраивает таблицу: удаляет могилы и при необходимости увеличивает размер
static void rehash(HashTable* table) {
    int newCapacity = table->capacity;
    if ((table->used + 1) * 20 > table->capacity * 7) {  // живых больше 35 то растём
        newCapacity = table->capacity * 2;
    }
    HashTable bigger;
    initHashTable(&bigger, table->mode, newCapacity, table->autoGrow);
    for (int i = 0; i < table->capacity; i++) {
        if (table->slots[i].state == SLOT_USED) {
            bool found;
            int index = locate(&bigger, table->slots[i].key, found);
            bigger.slots[index] = table->slots[i];
            bigger.used++;
        }
    }
    bigger.probes = table->probes;
    freeHashTable(table);
    *table = bigger;
}

InsertResult htInsert(HashTable* table, const std::string& key, const std::string& value, DNode* node) {
    bool found;
    int index = locate(table, key, found);
    if (found) {
        table->slots[index].value = value;
        table->slots[index].node = node;
        return HT_UPDATED;
    }
    if (table->autoGrow && (table->used + table->deleted + 1) * 10 > table->capacity * 7) {
        rehash(table);
        index = locate(table, key, found);
    }
    if (index < 0) {
        return HT_FULL;
    }
    Slot* slot = &table->slots[index];
    if (slot->state == SLOT_DELETED) {
        table->deleted--;
    }
    slot->key = key;
    slot->value = value;
    slot->node = node;
    slot->state = SLOT_USED;
    table->used++;
    return HT_ADDED;
}

Slot* htFind(HashTable* table, const std::string& key) {
    bool found;
    int index = locate(table, key, found);
    if (!found) {
        return nullptr;
    }
    return &table->slots[index];
}

bool htErase(HashTable* table, const std::string& key) {
    bool found;
    int index = locate(table, key, found);
    if (!found) {
        return false;
    }
    Slot* slot = &table->slots[index];
    slot->key = "";
    slot->value = "";
    slot->node = nullptr;
    slot->state = SLOT_DELETED;
    table->used--;
    table->deleted++;
    return true;
}

int htSize(const HashTable* table) {
    return table->used;
}

void printHashTable(const HashTable* table) {
    std::cout << "Table size = " << table->capacity << ", used = " << table->used
              << ", deleted = " << table->deleted << ", "
              << (table->mode == PROBE_LINEAR ? "linear probing" : "double hashing") << std::endl;
    for (int i = 0; i < table->capacity; i++) {
        const Slot* slot = &table->slots[i];
        if (slot->state == SLOT_USED) {
            std::cout << "  [" << i << "] " << slot->key;
            if (slot->value != "") {
                std::cout << " -> " << slot->value;
            }
            std::cout << std::endl;
        }
    }
}
