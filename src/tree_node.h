#ifndef COORDDESCENT_CODE_TREE_NODE_H_
#define COORDDESCENT_CODE_TREE_NODE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct TreeNode {
  double mass;
  double mass_left;
  double mass_right;
  struct TreeNode *left;
  struct TreeNode *right;
  struct TreeNode *parent;
} TreeNode;

double TreeNode_TotalMass(const TreeNode *t);

void TreeNode_MemoizeMass(TreeNode *t);

void TreeNode_UpdateMass(TreeNode *t, double mass);

void TreeNode_RescaleNode(TreeNode *node, double mass);

#endif  // COORDDESCENT_CODE_TREE_NODE_H_
