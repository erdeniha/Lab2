#pragma once

#include <string>

struct Node {
    std::string value;
    Node* next;
};

struct Stack {
    Node* head;
};

void initStack(Stack* stack);
void freeStack(Stack* stack);
void stackPush(Stack* stack, const std::string& value);
bool stackPop(Stack* stack, std::string& result);
bool stackPeek(const Stack* stack, std::string& result);
