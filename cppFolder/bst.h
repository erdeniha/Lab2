#pragma once
#include "array.h"
#include "tree.h"

bool bstInsert(Tree* tree, int64_t value);  
void bstRightEdge(const Tree* tree, Array* result); 
void printBst(const Tree* tree);

