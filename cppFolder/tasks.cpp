#include "tasks.h"
#include <iostream>
#include <sstream>
#include <cstdint>
#include "array.h"
#include "benchmark.h"
#include "bst.h"
#include "expr.h"
#include "hashtable.h"
#include "lru.h"
#include "partition.h"
#include "subsets.h"
#include "tree.h"

static int fail(const std::string& message) {
    std::cout << "ERROR: " << message << std::endl;
    return 1;
}

static bool isNumber(const std::string& s, size_t maxDigits) {
    if (s.empty() || s.size() > maxDigits) {
        return false;
    }
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i] < '0' || s[i] > '9') {
            return false;
        }
    }
    return true;
}

static bool parseInteger(const std::string& s, int64_t& value) {
    bool negative = !s.empty() && s[0] == '-';
    std::string digits = negative ? s.substr(1) : s;
    if (!isNumber(digits, 15)) {
        return false;
    }
    value = std::stoll(digits);
    if (negative) {
        value = -value;
    }
    return true;
}

static int taskExpr(const std::vector<std::string>& args) { // зад 1
    std::string text = "";
    for (size_t i = 0; i < args.size(); i++) {
        text += (i > 0 ? " " : "") + args[i];
    }
    int64_t result = 0;
    std::string error;
    if (!evalExpression(text, result, error)) {
        return fail(error);
    }
    std::cout << result << std::endl;
    return 0;
}



static int taskPartition(const std::vector<std::string>& args) { // зад 3
    size_t start = 0;
    int64_t wanted = 0;
    if (args.size() >= 2 && args[0] == "--sum") {
        if (!isNumber(args[1], 15) || std::stoll(args[1]) == 0) {
            return fail("--sum должен быть натуральным числом");
        }
        wanted = std::stoll(args[1]);
        start = 2;
    }

    Array input;
    initArray(&input);
    int64_t total = 0;
    for (size_t i = start; i < args.size(); i++) {
        if (!isNumber(args[i], 9) || std::stoll(args[i]) == 0) {
            freeArray(&input);
            return fail("'" + args[i] + "' не натуральное число");
        }
        arrayPush(&input, std::to_string(std::stoll(args[i])));
        total += std::stoll(args[i]);
    }
    int n = arrayLength(&input);
    if (n == 0) {
        freeArray(&input);
        return fail("Множество пустое");
    }

    int found = 0;
    for (int k = 2; k <= n; k++) {
        if (total % k != 0) {
            continue;
        }
        int64_t target = total / k;
        if (wanted != 0 && target != wanted) {
            continue;
        }
        Partition partition;
        if (findPartition(&input, target, &partition)) {
            std::cout << "Sum " << target << " (" << k << " subsets): ";
            printPartition(&partition);
            freePartition(&partition);
            found++;
        }
    }
    freeArray(&input);
    if (found == 0) {
        return fail("множество не может быть разделено на подмножества с равной суммой");
    }
    return 0;
}



static int taskSubsets(const std::vector<std::string>& args) {  // зад 4
    if (args.size() > 20) {
        return fail("слишком много элементов");
    }
    Array items, result;
    initArray(&items);
    initArray(&result);
    for (size_t i = 0; i < args.size(); i++) {
        arrayPush(&items, args[i]);
    }
    allSubsets(&items, &result);

    std::cout << "[";
    for (int i = 0; i < arrayLength(&result); i++) {
        std::string text;
        arrayGet(&result, i, text);
        std::cout << (i > 0 ? ", " : "") << text;
    }
    std::cout << "]" << std::endl;
    freeArray(&items);
    freeArray(&result);
    return 0;
}



static int taskBst(const std::vector<std::string>& args) {  // зад 5
    Tree tree;
    initTree(&tree);
    for (size_t i = 0; i < args.size(); i++) {
        int64_t value;
        if (!parseInteger(args[i], value)) {
            freeTree(&tree);
            return fail("'" + args[i] + "' не является целым числом");
        }
        if (!bstInsert(&tree, value)) {
            std::cout << "проигнорированные дубликаты: " << value << std::endl;
        }
    }
    printBst(&tree);

    Array edge;
    initArray(&edge);
    bstRightEdge(&tree, &edge);
    std::cout << "Правый край: ";
    for (int i = 0; i < arrayLength(&edge); i++) {
        std::string text;
        arrayGet(&edge, i, text);
        std::cout << (i > 0 ? ", " : "") << text;
    }
    std::cout << std::endl;
    freeArray(&edge);
    freeTree(&tree);
    return 0;
}

static int taskFold(const std::vector<std::string>& args) { // зад 6.2
    int size = 11;
    ProbeMode mode = PROBE_DOUBLE;
    std::vector<std::string> keys;
    for (size_t i = 0; i < args.size(); i++) {
        if (args[i] == "--size" && i + 1 < args.size()) {
            if (!isNumber(args[i + 1], 6) || std::stoi(args[i + 1]) < 5) {
                return fail("--size должно быть число от 5");
            }
            size = std::stoi(args[++i]);
        } else if (args[i] == "--mode" && i + 1 < args.size()) {
            std::string m = args[++i];
            if (m != "linear" && m != "double") {
                return fail("--mode должен быть линейным или двойным");
            }
            mode = (m == "linear") ? PROBE_LINEAR : PROBE_DOUBLE;
        } else {
            keys.push_back(args[i]);
        }
    }
    if (keys.empty()) {
        return fail("ключи не выданы");
    }

    HashTable table;
    initHashTable(&table, mode, size, false);
    for (size_t i = 0; i < keys.size(); i++) {
        int64_t sum = foldKey(keys[i], 3);
        std::cout << keys[i] << ": " << foldParts(keys[i], 3) << " = " << sum
                  << ", home slot = " << sum << " mod " << table.capacity << " = " << sum % table.capacity
                  << std::endl;
        if (htInsert(&table, keys[i], "", nullptr) == HT_FULL) {
            std::cout << "  Таблица полная, '" << keys[i] << "' не был добавлен" << std::endl;
        }
    }
    printHashTable(&table);
    freeHashTable(&table);
    return 0;
}

static int taskLru(const std::vector<std::string>& args) { // зад 7
    if (args.size() < 1 || !isNumber(args[0], 6) || std::stoi(args[0]) < 1) {
        return fail("Первым аргументом должен быть объем кэша (число от 1).");
    }
    std::vector<std::string> t;
    for (size_t i = 1; i < args.size(); i++) {  
        std::istringstream stream(args[i]);
        std::string word;
        while (stream >> word) {
            t.push_back(word);
        }
    }

    LRUCache cache;
    initLRU(&cache, std::stoi(args[0]));
    std::string output = "";
    size_t i = 0;
    int code = 0;
    while (i < t.size()) {
        if (t[i] == "SET" && i + 2 < t.size()) {
            lruSet(&cache, t[i + 1], t[i + 2]);
            i += 3;
        } else if (t[i] == "GET" && i + 1 < t.size()) {
            std::string value;
            if (!lruGet(&cache, t[i + 1], value)) {
                value = "-1";
            }
            output += (output.empty() ? "" : " ") + value;
            i += 2;
        } else {
            code = fail("Неверный запрос в слове '" + t[i] + "' (ожидался SET x y или GET x)");
            break;
        }
    }
    if (code == 0) {
        std::cout << output << std::endl;
    }
    freeLRU(&cache);
    return code;
}

static int taskBench(const std::vector<std::string>& args) { // зад 6.1
    (void)args;
    runBenchmark();
    return 0;
}

struct Task {
    const char* name;
    int (*run)(const std::vector<std::string>& args);
};

static const Task tasks[] = {
    {"expr", taskExpr},
    {"partition", taskPartition},
    {"subsets", taskSubsets},
    {"bst", taskBst},
    {"fold", taskFold},
    {"lru", taskLru},
    {"bench", taskBench}
};

int runTask(const std::string& name, const std::vector<std::string>& args, bool& found) {
    int count = sizeof(tasks) / sizeof(tasks[0]);
    for (int i = 0; i < count; i++) {
        if (name == tasks[i].name) {
            found = true;
            return tasks[i].run(args);
        }
    }
    found = false;
    return 1;
}

void printUsage() {
    std::cout << "Задача №1     ./lab2 expr '<expression>'" << std::endl;
    std::cout << "Задача №2     ./lab2 --file <file> --query '<COMMAND args>'   (SETADD, SETDEL, SET_AT, ...)" << std::endl;
    std::cout << "Задача №3     ./lab2 partition [--sum S] n1 n2 ..." << std::endl;
    std::cout << "Задача №4     ./lab2 subsets e1 e2 ..." << std::endl;
    std::cout << "Задача №5     ./lab2 bst v1 v2 ..." << std::endl;
    std::cout << "Задача №6.1   ./lab2 bench" << std::endl;
    std::cout << "Задача №6.2   ./lab2 fold [--size N] [--mode linear|double] key ..." << std::endl;
    std::cout << "Задача №7     ./lab2 lru <capacity> '<SET x y | GET x ...>'" << std::endl;
}
