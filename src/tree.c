#pragma once
#include "root.h"

typedef struct {
    int parentIdx;
    int firstChildIdx;
    int childCount;
    Move move;
    float value;
} TreeNode;

TreeNode *newTreeNode(TreeNode *tree, int parentIdx, Move move, float val) {
    // create a new tree node with an associated move, the value of the position after this move will be calculated later
    TreeNode newNode;
    newNode.move = move;
    newNode.value = val;
    newNode.childCount = 0;
    newNode.firstChildIdx = -1;
    // set the root node's parentIdx to -1 if there are no nodes in the tree
    if(arrlen(tree) == 0)
        newNode.parentIdx = -1;
    else {
        newNode.parentIdx = parentIdx;
        TreeNode *parent = &tree[parentIdx];

        // TODO: insertion
        // checck the assertion logic, some stuff isnt working here
        {
            size_t treeLen = arrlen(tree);
            TreeNode lastNode = tree[treeLen - 1];
            assert(lastNode.parentIdx <= parentIdx && "insertion attempted");
        }

        if(parent->childCount == 0) {
            parent->firstChildIdx = arrlen(tree);
        }
        ++parent->childCount;
    }
    arrpush(tree, newNode);
    return tree;
}
