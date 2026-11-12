#include "bst.h"

#include <iostream>

bool bstInsert(Tree* tree, int64_t value) {
    TreeNode** place = &tree->root;  
    while (*place != nullptr) {
        int64_t current = std::stoll((*place)->value);
        if (value == current) {
            return false;
        }
        place = (value < current) ? &(*place)->left : &(*place)->right;
    }
    *place = new TreeNode{std::to_string(value), nullptr, nullptr};
    tree->size++;
    return true;
}   

static void rightEdge(const TreeNode* node, int depth, Array* result) {
    if (node == nullptr) {
        return;
    }
    if (depth == arrayLength(result)) {
        arrayPush(result, node->value);
    }
    rightEdge(node->right, depth + 1, result);
    rightEdge(node->left, depth + 1, result);
}

void bstRightEdge(const Tree* tree, Array* result) {
    rightEdge(tree->root, 0, result);
}

static void printSideways(const TreeNode* node, int depth) {
    if (node == nullptr) {
        return;
    }
    printSideways(node->right, depth + 1);
    std::cout << std::string(depth * 4, ' ') << node->value << std::endl;
    printSideways(node->left, depth + 1);
}

void printBst(const Tree* tree) {
    if (tree->root == nullptr) {
        std::cout << "(Пустое дерево)" << std::endl;
        return;
    }
    std::cout << "Дерево :" << std::endl;
    printSideways(tree->root, 0);
}
