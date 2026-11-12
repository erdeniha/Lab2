#include "commands.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "set.h"

enum Op {
    OP_SET_ADD,
    OP_SET_DEL,
    OP_SET_AT
};

struct Command {
    const char* name;
    Op op;
    bool creates;
};

static const Command commands[] = {
    {"SETADD", OP_SET_ADD, true},
    {"SETDEL", OP_SET_DEL, false},
    {"SET_AT", OP_SET_AT,  false}
};

static const Command* findCommand(const std::string& name) {
    int count = sizeof(commands) / sizeof(commands[0]);
    for (int i = 0; i < count; i++) {
        if (name == commands[i].name) {
            return &commands[i];
        }
    }
    return nullptr;
}

static std::vector<std::string> split(const std::string& text) {
    std::vector<std::string> parts;
    std::istringstream stream(text);
    std::string word;
    while (stream >> word) {
        parts.push_back(word);
    }
    return parts;
}

static std::string toUpper(std::string s) {
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i] >= 'a' && s[i] <= 'z') {
            s[i] = s[i] - 'a' + 'A';
        }
    }
    return s;
}

static bool fail(const std::string& message) {
    std::cout << "-> ОШИБКА: " << message << std::endl;
    return false;
}

static std::vector<std::string> readLines(const std::string& file) {
    std::vector<std::string> lines;
    std::ifstream in(file);
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty()) {
            lines.push_back(line);
        }
    }
    return lines;
}

static bool writeLines(const std::string& file, const std::vector<std::string>& lines) {
    std::ofstream out(file);
    if (!out) {
        return false;
    }
    for (size_t i = 0; i < lines.size(); i++) {
        out << lines[i] << "\n";
    }
    return true;
}

static int findLine(const std::vector<std::string>& lines, const std::string& name) {
    for (size_t i = 0; i < lines.size(); i++) {
        std::vector<std::string> fields = split(lines[i]);
        if (!fields.empty() && fields[0] == name) {
            return (int)i;
        }
    }
    return -1;
}

static bool doSet(Op op, const std::string& element, std::vector<std::string>& values, bool& changed) {
    Set set;
    initSet(&set);
    for (size_t i = 0; i < values.size(); i++) {
        setAdd(&set, values[i]);
    }

    bool result = false;
    switch (op) {
        case OP_SET_ADD:
            result = setAdd(&set, element);
            changed = result;
            break;
        case OP_SET_DEL:
            result = setDel(&set, element);
            changed = result;
            break;
        case OP_SET_AT:
            result = setAt(&set, element);
            break;
    }

    if (changed) {
        values.clear();
        setToValues(&set, values);
    }
    freeSet(&set);
    return result;
}

bool runQuery(const std::string& file, const std::string& query) {
    std::vector<std::string> t = split(query);
    if (t.empty()) {
        return fail("пустой запрос");
    }
    t[0] = toUpper(t[0]);

    const Command* cmd = findCommand(t[0]);
    if (cmd == nullptr) {
        return fail("неизвестная команда: " + t[0]);
    }
    if (t.size() != 3) {
        return fail("неверное число аргументов у команды " + t[0]);
    }

    std::vector<std::string> lines = readLines(file);
    std::string name = t[1];
    int lineIndex = findLine(lines, name);

    if (lineIndex < 0 && !cmd->creates) {
        return fail("множество '" + name + "' не найдено");
    }

    std::vector<std::string> values;
    if (lineIndex >= 0) {
        std::vector<std::string> fields = split(lines[lineIndex]);
        values.assign(fields.begin() + 1, fields.end());
    }

    bool changed = false;
    bool result = doSet(cmd->op, t[2], values, changed);
    std::cout << "-> " << (result ? "TRUE" : "FALSE") << std::endl;

    if (changed) {
        std::string line = name;
        for (size_t i = 0; i < values.size(); i++) {
            line += " " + values[i];
        }
        if (lineIndex >= 0) {
            lines[lineIndex] = line;
        } else {
            lines.push_back(line);
        }
        if (!writeLines(file, lines)) {
            return fail("не удалось записать файл " + file);
        }
    }
    return true;
}
