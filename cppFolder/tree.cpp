#include "tree.h"

void initTree(Tree* tree) {
    tree->root = nullptr;
    tree->size = 0;
}

static void freeNode(TreeNode* node) {
    if (node == nullptr) {
        return;
    }
    freeNode(node->left);
    freeNode(node->right);
    delete node;
}

void freeTree(Tree* tree) {
    freeNode(tree->root);
    tree->root = nullptr;
    tree->size = 0;
}
