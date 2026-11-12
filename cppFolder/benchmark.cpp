#include "benchmark.h"

#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include "hashtable.h"
#include "set.h"

static const int TABLE_SIZE = 10007;
static const int TRIALS = 5;
static const int MISSING_COUNT = 3000;
static const double LOADS[] = {0.1, 0.3, 0.5, 0.7, 0.8, 0.9, 0.95};
static const int LOAD_COUNT = sizeof(LOADS) / sizeof(LOADS[0]);

struct Measure {
    double found;
    double miss;
    double longest;
    double nanos;
};

static std::string makeKey(std::mt19937& rng, bool numeric) {
    if (numeric) {
        return std::to_string(100000000 + rng() % 900000000);
    }
    static const char alphabet[] = "abcdefghijklmnopqrstuvwxyz0123456789";
    std::string key;
    for (int i = 0; i < 8; i++) {
        key += alphabet[rng() % (sizeof(alphabet) - 1)];
    }
    return key;
}

static Measure measureTable(HashTable* table, const std::vector<std::string>& keys, int count, const std::vector<std::string>& missing) {
    Measure result = {0, 0, 0, 0};

    auto before = table->probes;
    for (int i = 0; i < count; i++) {
        auto start = table->probes;
        htFind(table, keys[i]);
        double length = (double)(table->probes - start);
        if (length > result.longest) {
            result.longest = length;
        }
    }
    result.found = (double)(table->probes - before) / count;

    before = table->probes;
    for (size_t i = 0; i < missing.size(); i++) {
        htFind(table, missing[i]);
    }
    result.miss = (double)(table->probes - before) / missing.size();

    auto begin = std::chrono::steady_clock::now();
    for (int i = 0; i < count; i++) {
        htFind(table, keys[i]);
    }
    auto end = std::chrono::steady_clock::now();
    result.nanos = std::chrono::duration<double, std::nano>(end - begin).count() / count;
    return result;
}

static void runExperiment(bool numeric) {
    int maxKeys = (int)(LOADS[LOAD_COUNT - 1] * TABLE_SIZE);
    ProbeMode modes[] = {PROBE_LINEAR, PROBE_DOUBLE};
    Measure total[2][LOAD_COUNT] = {};

    for (int trial = 0; trial < TRIALS; trial++) {
        std::mt19937 rng(trial + 1);
        Set used;
        initSet(&used);
        std::vector<std::string> keys;
        while ((int)keys.size() < maxKeys) {
            std::string key = makeKey(rng, numeric);
            if (setAdd(&used, key)) {
                keys.push_back(key);
            }
        }
        std::vector<std::string> missing;
        while ((int)missing.size() < MISSING_COUNT) {
            std::string key = makeKey(rng, numeric);
            if (!setAt(&used, key)) {
                missing.push_back(key);
            }
        }
        freeSet(&used);

        for (int m = 0; m < 2; m++) {
            HashTable table;
            initHashTable(&table, modes[m], TABLE_SIZE, false);
            int inserted = 0;
            for (int l = 0; l < LOAD_COUNT; l++) {
                int target = (int)(LOADS[l] * TABLE_SIZE);
                for (; inserted < target; inserted++) {
                    htInsert(&table, keys[inserted], "", nullptr);
                }
                Measure one = measureTable(&table, keys, inserted, missing);
                total[m][l].found += one.found;
                total[m][l].miss += one.miss;
                total[m][l].longest += one.longest;
                total[m][l].nanos += one.nanos;
            }
            freeHashTable(&table);
        }
    }

    std::cout << (numeric ? "Ключи: 9-значные числа" : "Ключи: случайные строки по 8 символов") 
    << ", таблица " << TABLE_SIZE << " слотов, среднее по " << TRIALS << " запускам" << std::endl;
    const char* names[] = {"Линейное апробирование", "Двойное хеширование"};
    for (int m = 0; m < 2; m++) {
        std::cout << names[m] << std::endl;
        std::cout << "нагрузка  ключей   найден (теория)   не найден (теория)   макс.проб   нс/поиск" << std::endl;
        for (int l = 0; l < LOAD_COUNT; l++) {
            int inserted = (int)(LOADS[l] * TABLE_SIZE);
            double a = (double)inserted / TABLE_SIZE;
            double theoryFound = 0;
            double theoryMiss = 0;
            if (m == 0) {
                theoryFound = 0.5 * (1 + 1 / (1 - a));
                theoryMiss = 0.5 * (1 + 1 / ((1 - a) * (1 - a)));
            } else {
                theoryFound = (1 / a) * std::log(1 / (1 - a));
                theoryMiss = 1 / (1 - a);
            }
            std::cout << std::fixed << std::setprecision(2)
                      << std::setw(8) << LOADS[l] << std::setw(9) << inserted
                      << std::setw(11) << total[m][l].found / TRIALS << " (" << theoryFound << ")"
                      << std::setw(11) << total[m][l].miss / TRIALS << " (" << theoryMiss << ")"
                      << std::setw(12) << std::setprecision(0) << total[m][l].longest / TRIALS
                      << std::setw(12) << std::setprecision(1) << total[m][l].nanos / TRIALS << std::endl;
        }
    }
    std::cout << std::endl;
}

void runBenchmark() {
    runExperiment(false);
    runExperiment(true);
}