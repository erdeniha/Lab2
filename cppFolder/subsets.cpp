#include "subsets.h"
#include <algorithm>
#include <vector>
#include "set.h"

static void build(const Array* items, int start, int size, std::vector<int>& picked, Set* seen, Array* result) {
    if ((int)picked.size() == size) {
        std::vector<std::string> chosen;
        std::string text = "{";
        for (size_t i = 0; i < picked.size(); i++) {
            std::string item;
            arrayGet(items, picked[i], item);
            chosen.push_back(item);
            text += (i > 0 ? ", " : "") + item;
        }
        text += "}";
        std::sort(chosen.begin(), chosen.end());
        std::string key = "";
        for (size_t i = 0; i < chosen.size(); i++) {
            key += chosen[i] + "\x1f";
        }
        if (setAdd(seen, key)) {
            arrayPush(result, text);
        }
        return;
    }
    int n = arrayLength(items);
    int still = size - (int)picked.size();
    for (int i = start; i <= n - still; i++) {
        picked.push_back(i);
        build(items, i + 1, size, picked, seen, result);
        picked.pop_back();
    }
}

void allSubsets(const Array* items, Array* result) {
    Set seen;
    initSet(&seen);
    std::vector<int> picked;
    for (int size = 0; size <= arrayLength(items); size++) {
        build(items, 0, size, picked, &seen, result);
    }
    freeSet(&seen);
}
