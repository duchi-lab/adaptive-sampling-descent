#include "probability_tree.h"

const double kPrecision = 0.000001;

ProbabilityTree * ConstructProbabilityTree(const size_t n, const double p_min) {
  ProbabilityTree *tree =
      (ProbabilityTree *) malloc(sizeof(ProbabilityTree));
  tree->tree = (TreeNode *) calloc(n, sizeof(TreeNode));
  tree->root = tree->tree;
  tree->p_min = p_min;
  tree->length = n;

  // insert weights into tree.
  int left_index = 0;
  int right_index = 0;
  int parent_index = 0;
  TreeNode *node = NULL;
  for (int i = 0; i < n; ++i) {
    node = tree->tree + i;
    node->mass = 1.0 / n;
    left_index = 2*i + 1;
    right_index = 2*i + 2;
    parent_index = (i - 1) / 2;
    if (left_index < n) {
      node->left = tree->tree + left_index;
    }
    if (right_index < n) {
      node->right = tree->tree + right_index;
    }
    if (parent_index >= 0 && parent_index != i) {
      node->parent = tree->tree + parent_index;
    } else {
      node->parent = NULL;
    }
  }

  for (int i = 0; i < n; ++i) {
    node = tree->tree + n-i-1;
    if (node->left != NULL && node->left->parent != node) {
      printf("Left error\n");
      exit(12);
    }
    if (node->right != NULL && node->right->parent != node) {
      printf("Right error\n");
      exit(12);
    }
  }

  // Adjust left / right child weights.
  for (int i = 0; i < n; ++i) {
    node = tree->tree + n-i-1;
    TreeNode_MemoizeMass(node);
  }

  return(tree);
}


ProbabilityTree * ConstructProbabilityTreeWithWeights(const size_t n, const double p_min, const double * p) {
  ProbabilityTree *tree =
      (ProbabilityTree *) malloc(sizeof(ProbabilityTree));
  tree->tree = (TreeNode *) calloc(n, sizeof(TreeNode));
  tree->root = tree->tree;
  tree->p_min = p_min;
  tree->length = n;

  // insert weights into tree.
  int left_index = 0;
  int right_index = 0;
  int parent_index = 0;
  TreeNode *node = NULL;
  for (int i = 0; i < n; ++i) {
    node = tree->tree + i;
    node->mass = p[i];
    left_index = 2*i + 1;
    right_index = 2*i + 2;
    parent_index = (i - 1) / 2;
    if (left_index < n) {
      node->left = tree->tree + left_index;
    }
    if (right_index < n) {
      node->right = tree->tree + right_index;
    }
    if (parent_index >= 0 && parent_index != i) {
      node->parent = tree->tree + parent_index;
    } else {
      node->parent = NULL;
    }
  }

  for (int i = 0; i < n; ++i) {
    node = tree->tree + n-i-1;
    if (node->left != NULL && node->left->parent != node) {
      printf("Left error\n");
      exit(12);
    }
    if (node->right != NULL && node->right->parent != node) {
      printf("Right error\n");
      exit(12);
    }
  }

  // Adjust left / right child weights.
  for (int i = 0; i < n; ++i) {
    node = tree->tree + n-i-1;
    TreeNode_MemoizeMass(node);
  }

  return(tree);
}

double ProbabilityTree_GetMass(const ProbabilityTree *t) {
  return(TreeNode_TotalMass(t->root));
}

double ProbabilityTree_ProbabilityAtIndex(const ProbabilityTree *t,
                   const int64_t index) {
  if (index >= 0 && index < t->length) {
    return(t->tree[index].mass / ProbabilityTree_GetMass(t));
  } else {
    return(-1.0);
  }
}

void ProbabilityTree_Rescale(ProbabilityTree *tree) {
  double mass = 0;
  for (int i = 0; i < tree->length; ++i) {
    mass += tree->tree[i].mass;
  }
  for (int i = 0; i < tree->length; ++i) {
    TreeNode_RescaleNode(tree->tree + i, mass);
  }
  for (int i = tree->length - 1; i >= 0; --i) {
    TreeNode_MemoizeMass(tree->tree + i);
  }
}

void ProbabilityTree_KLProject(ProbabilityTree *tree,
               const int64_t index,
               double p_new) {
  const double mass = ProbabilityTree_GetMass(tree);

  TreeNode *node = tree->tree + index;

  const double weight_old = node->mass;

  p_new *= mass;
  const double z = mass - weight_old + p_new;

  if (p_new / z >= tree->p_min) {
    TreeNode_UpdateMass(node, p_new);
  } else {
    TreeNode_UpdateMass(node, tree->p_min * (mass - weight_old) / (1 - tree->p_min));
  }

  if (mass < kPrecision / tree->length) {
    ProbabilityTree_Rescale(tree);
  }
}

const int64_t ProbabilityTree_SampleIndex(ProbabilityTree *tree) {
  const TreeNode* curr_node = tree->root;
  const double mass = ProbabilityTree_GetMass(tree);
  double coin = mass * (double)(rand())/RAND_MAX; // coin toss
  double temp_left, temp_weight;
  while ((curr_node->left != NULL) || (curr_node->right != NULL)) {
    /*printf("Coin: %.5f, Mass: %.5f, Left: %.5f, Self: %.5f, Right: %.5f\n",
           coin, mass, TotalMass(curr_node->left),
           curr_node->mass, TotalMass(curr_node->right));*/
    // curr_node is not a leaf node by the above condition
    if (curr_node->left != NULL) {
      temp_left  = TreeNode_TotalMass(curr_node->left);
      temp_weight = curr_node->mass;
    }
    else {
      temp_left = 0;
      temp_weight = curr_node->mass;
    }
    /*printf("Coin (%.5f) <= temp_weight (%.5f)? %d\n", coin, temp_weight,
           (uint32_t)(coin <= temp_weight));
    printf("Coin (%.5f) <= temp_weight+temp_left (%.5f)? %d\n", coin,
    temp_weight + temp_left, (uint32_t)(coin <= temp_weight + temp_left));*/
    if (coin <= temp_weight) {
      break;
    }
    else if (coin <= (temp_weight + temp_left)) {
      coin -= temp_weight;
      curr_node = curr_node->left;
    }
    else {
      coin -= temp_weight + temp_left;
      if (curr_node->right) {
        curr_node = curr_node->right;
      } else {
        // This is probably an error, so we will raise a warning, but chances are it's
        // a result of floating point issues, so assign to left.
        printf("Error: The mass of the coin was bigger than the mass of the tree.\n");
        curr_node = curr_node->left;
      }
    }
  }
  return curr_node - tree->tree;
}
