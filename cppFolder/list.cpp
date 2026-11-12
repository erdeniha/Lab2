#include "list.h"

void initDList(DList* list) {
    list->head = nullptr;
    list->tail = nullptr;
}

void freeDList(DList* list) {
    while (list->head != nullptr) {
        DNode* temp = list->head;
        list->head = temp->next;
        delete temp;
    }
    list->tail = nullptr;
}

void dAddHead(DList* list, const std::string& value) {
    DNode* node = new DNode{value, nullptr, list->head};
    if (list->head != nullptr) {
        list->head->prev = node;
    } else {
        list->tail = node;
    }
    list->head = node;
}

bool dDelTail(DList* list, std::string& result) {
    if (list->tail == nullptr) {
        return false;
    }
    DNode* node = list->tail;
    result = node->value;
    list->tail = node->prev;
    if (list->tail != nullptr) {
        list->tail->next = nullptr;
    } else {
        list->head = nullptr;
    }
    delete node;
    return true;
}

void dMoveToHead(DList* list, DNode* node) {
    if (node == list->head) {
        return;
    }
    node->prev->next = node->next;
    if (node->next != nullptr) {
        node->next->prev = node->prev;
    } else {
        list->tail = node->prev;
    }
    node->prev = nullptr;
    node->next = list->head;
    list->head->prev = node;
    list->head = node;
}
