#include "stack.h"

void initStack(Stack* stack) {
    stack->head = nullptr;
}

void freeStack(Stack* stack) {
    while (stack->head != nullptr) {
        Node* temp = stack->head;
        stack->head = temp->next;
        delete temp;
    }
}

void stackPush(Stack* stack, const std::string& value) {
    stack->head = new Node{value, stack->head};
}

bool stackPop(Stack* stack, std::string& result) {
    if (stack->head == nullptr) {
        return false;
    }
    Node* temp = stack->head;
    result = temp->value;
    stack->head = temp->next;
    delete temp;
    return true;
}

bool stackPeek(const Stack* stack, std::string& result) {
    if (stack->head == nullptr) {
        return false;
    }
    result = stack->head->value;
    return true;
}
