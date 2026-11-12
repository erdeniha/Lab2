#include <iostream>
#include <string>
#include <vector>
#include "commands.h"
#include "tasks.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage();
        return 1;
    }
    std::string first = argv[1];

    if (first.size() > 1 && first[0] == '-') {  
        std::string file;
        std::string query;
        for (int i = 1; i + 1 < argc; i += 2) {
            std::string key = argv[i];
            if (key == "--file") {
                file = argv[i + 1];
            } else if (key == "--query") {
                query = argv[i + 1];
            }
        }
        if (file.empty() || query.empty()) {
            printUsage();
            return 1;
        }
        return runQuery(file, query) ? 0 : 1;
    }

    std::vector<std::string> args;
    for (int i = 2; i < argc; i++) {
        args.push_back(argv[i]);
    }
    bool found = false;
    int code = runTask(first, args, found);
    if (!found) {
        std::cout << "ERROR: неизвестная задача: " << first << std::endl;
        printUsage();
        return 1;
    }
    return code;
}
