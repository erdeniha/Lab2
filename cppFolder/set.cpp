#include "set.h"

#include <algorithm>

void initSet(Set* set) {
    initHashTable(&set->table, PROBE_DOUBLE, 11, true);
}

void freeSet(Set* set) {
    freeHashTable(&set->table);
}

bool setAdd(Set* set, const std::string& value) {
    return htInsert(&set->table, value, "", nullptr) == HT_ADDED;
}

bool setDel(Set* set, const std::string& value) {
    return htErase(&set->table, value);
}

bool setAt(Set* set, const std::string& value) {
    return htFind(&set->table, value) != nullptr;
}

void setToValues(const Set* set, std::vector<std::string>& values) {
    for (int i = 0; i < set->table.capacity; i++) {
        if (set->table.slots[i].state == SLOT_USED) {
            values.push_back(set->table.slots[i].key);
        }
    }
    std::sort(values.begin(), values.end());
}
