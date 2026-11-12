#include "partition.h"
#include <iostream>
#include <vector>
#include <cstdint>

static bool place(const std::vector<uint64_t>& numbers, int index, uint64_t target, std::vector<uint64_t>& sums, std::vector<int>& basket) {
    if (index == (int)numbers.size()) {
        return true; 
    }
    for (size_t b = 0; b < sums.size(); b++) {
        if (sums[b] + numbers[index] > target) {
            continue;
        }
        bool sameAsBefore = false;
        for (size_t c = 0; c < b; c++) {
            if (sums[c] == sums[b]) {
                sameAsBefore = true;
            }
        }
        if (sameAsBefore) {
            continue;
        }
        sums[b] += numbers[index];
        basket[index] = (int)b;
        if (place(numbers, index + 1, target, sums, basket)) {
            return true;
        }
        sums[b] -= numbers[index];
    }
    return false;
}

bool findPartition(const Array* input, uint64_t target, Partition* result) {
    result->groups = nullptr;
    result->count = 0;

    int n = arrayLength(input);
    std::vector<uint64_t> numbers;
    uint64_t total = 0;
    for (int i = 0; i < n; i++) {
        std::string text;
        arrayGet(input, i, text);
        uint64_t value = std::stoll(text);
        if (value > target) {
            return false; 
        }
        numbers.push_back(value);
        total += value;
    }
    if (target <= 0 || total % target != 0) {
        return false;
    }
    int k = (int)(total / target);
    if (k < 2 || k > n) {
        return false;
    }

    std::vector<uint64_t> sums(k, 0);
    std::vector<int> basket(n, 0);
    if (!place(numbers, 0, target, sums, basket)) {
        return false;
    }

    result->count = k;
    result->groups = new Array[k];
    for (int b = 0; b < k; b++) {
        initArray(&result->groups[b]);
    }
    for (int i = 0; i < n; i++) {
        std::string text;
        arrayGet(input, i, text);
        arrayPush(&result->groups[basket[i]], text);
    }
    return true;
}

void freePartition(Partition* partition) {
    for (int b = 0; b < partition->count; b++) {
        freeArray(&partition->groups[b]);
    }
    delete[] partition->groups;
    partition->groups = nullptr;
    partition->count = 0;
}

void printPartition(const Partition* partition) {
    for (int b = 0; b < partition->count; b++) {
        if (b > 0) {
            std::cout << " ";
        }
        std::cout << "{";
        for (int i = 0; i < arrayLength(&partition->groups[b]); i++) {
            std::string text;
            arrayGet(&partition->groups[b], i, text);
            std::cout << (i > 0 ? ", " : "") << text;
        }
        std::cout << "}";
    }
    std::cout << std::endl;
}
