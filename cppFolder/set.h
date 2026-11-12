#pragma once

#include <string>
#include <vector>

#include "hashtable.h"

struct Set {
    HashTable table;
};

void initSet(Set* set);
void freeSet(Set* set);
bool setAdd(Set* set, const std::string& value);
bool setDel(Set* set, const std::string& value);
bool setAt(Set* set, const std::string& value);
void setToValues(const Set* set, std::vector<std::string>& values);
