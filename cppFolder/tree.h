#pragma once

#include <string>

struct TreeNode {
    std::string value;
    TreeNode* left;
    TreeNode* right;
};

struct Tree {
    TreeNode* root;
    int size;
};

void initTree(Tree* tree);
void freeTree(Tree* tree);
