#pragma once
#include "array.h"
#include <cstdint>

struct Partition {
    Array* groups;  
    int count;
};

bool findPartition(const Array* input, uint64_t target, Partition* result);
void freePartition(Partition* partition);
void printPartition(const Partition* partition);

