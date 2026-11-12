#pragma once

#include <string>

struct DNode {
    std::string value;
    DNode* prev;
    DNode* next;
};

struct DList {
    DNode* head;
    DNode* tail;
};

void initDList(DList* list);
void freeDList(DList* list);
void dAddHead(DList* list, const std::string& value);
bool dDelTail(DList* list, std::string& result);
void dMoveToHead(DList* list, DNode* node);
