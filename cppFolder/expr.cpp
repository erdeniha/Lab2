#include "expr.h"
#include "stack.h"
static const int64_t LIMIT = 2000000000LL;

static int priority(const std::string& op) {
    if (op == "~") {
        return 3;
    }
    if (op == "*") {
        return 2;
    }
    if (op == "+" || op == "-") {
        return 1;
    }
    return 0;
}

static bool applyTop(Stack* numbers, Stack* operations, std::string& error) {
    std::string op, left, right;
    if (!stackPop(operations, op) || !stackPop(numbers, right)) {
        error = "Недостаточно операндов";
        return false;
    }
    int64_t b = std::stoll(right);
    int64_t value = 0;
    if (op == "~") {
        value = -b;
    } else {
        if (!stackPop(numbers, left)) {
            error = "Недостаточно операндов";
            return false;
        }
        int64_t a = std::stoll(left);
        switch (op[0]) {
            case '+': value = a + b; break;
            case '-': value = a - b; break;
            case '*': value = a * b; break; 
            default: break;
        }
    }
    if (value > LIMIT || value < -LIMIT) {
        error = "Значение вышло за лимит";
        return false;
    }
    stackPush(numbers, std::to_string(value));
    return true;
}

static bool calculate(const std::string& text, Stack* numbers, Stack* operations, int64_t& result, std::string& error) {
    bool expectOperand = true;  // сейчас должно идти число, ( или унарный знак
    size_t i = 0;
    while (i < text.size()) {
        char c = text[i];
        std::string where = " в позиции " + std::to_string(i + 1);

        if (c == ' ' || c == '\t') {
            i++;
        } else if (c >= '0' && c <= '9') {
            if (!expectOperand) {
                error = "Оператор ожидался перед числом" + where;
                return false;
            }
            int64_t number = 0;
            while (i < text.size() && text[i] >= '0' && text[i] <= '9') {
                number = number * 10 + (text[i] - '0');
                if (number > LIMIT) {
                    error = "Число вышло за лимит" + where;
                    return false;
                }
                i++;
            }
            stackPush(numbers, std::to_string(number));
            expectOperand = false;
        } else if (c == '(') {
            if (!expectOperand) {
                error = "Оператор ожидался перед '('" + where;
                return false;
            }
            stackPush(operations, "(");
            i++;
        } else if (c == ')') {
            if (expectOperand) {
                error = "Операнд ожидался перед ')'" + where;
                return false;
            }
            std::string top;
            while (stackPeek(operations, top) && top != "(") {
                if (!applyTop(numbers, operations, error)) {
                    return false;
                }
            }
            if (!stackPop(operations, top)) {
                error = "unbalanced ')'" + where;
                return false;
            }
            i++;
        } else if (c == '+' || c == '-' || c == '*') {
            std::string op(1, c);
            if (expectOperand) {
                if (c == '*') {
                    error = "Операнд ожидался перед '*'" + where;
                    return false;
                }
                if (c == '-') {
                    stackPush(operations, "~");
                }
            } else {
                std::string top;
                while (stackPeek(operations, top) && priority(top) >= priority(op)) {
                    if (!applyTop(numbers, operations, error)) {
                        return false;
                    }
                }
                stackPush(operations, op);
                expectOperand = true;
            }
            i++;
        } else {
            error = std::string("Неожиданный символ '") + c + "'" + where;
            return false;
        }
    }

    if (expectOperand) {
        error = text.find_first_not_of(" \t") == std::string::npos ? "Пустое выражение" : "Выражение является неполным";
        return false;
    }
    std::string top;
    while (stackPeek(operations, top)) {
        if (top == "(") {
            error = "несбалансированный '('";
            return false;
        }
        if (!applyTop(numbers, operations, error)) {
            return false;
        }
    }
    std::string answer;
    stackPop(numbers, answer);
    result = std::stoll(answer);
    return true;
}

bool evalExpression(const std::string& text, int64_t& result, std::string& error) {
    Stack numbers;
    Stack operations;
    initStack(&numbers);
    initStack(&operations);
    bool ok = calculate(text, &numbers, &operations, result, error);
    freeStack(&numbers);
    freeStack(&operations);
    return ok;
}
