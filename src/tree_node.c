#include "tree_node.h"

void TreeNode_MemoizeMass(TreeNode *t) {
  if (!t) {
    // Probably should raise a warning?
    return;
  }
  if (t->left) {
    t->mass_left = TreeNode_TotalMass(t->left);
  } else {
    t->mass_left = 0;
  }
  if (t->right) {
    t->mass_right = TreeNode_TotalMass(t->right);
  } else {
    t->mass_right = 0;
  }
}

void TreeNode_RescaleNode(TreeNode *node, double mass) {
  node->mass /= mass;
}

double TreeNode_TotalMass(const TreeNode *t) {
  if (!t) {
    return(0.0);
  }
  return(t->mass + t->mass_left + t->mass_right);
}

void TreeNode_UpdateMass(TreeNode *t, double mass) {
  t->mass = mass;
  while (t) {
    TreeNode_MemoizeMass(t);
    /*printf("[UM] Left: %.5f, Self: %.5f, Right: %.5f, Total: %.5f\n",
           TotalMass(t->left), t->mass, TotalMass(t->right),
           TotalMass(t));*/
    t = t->parent;
  }
}
