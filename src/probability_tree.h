#ifndef COORDDESCENT_CODE_PROBABILITY_TREE_H_
#define COORDDESCENT_CODE_PROBABILITY_TREE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "tree_node.h"

typedef struct ProbabilityTree {
  TreeNode *tree;
  TreeNode *root;
  size_t length;
  double p_min;
} ProbabilityTree;

ProbabilityTree * ConstructProbabilityTree(const size_t n, const double p_min);

double ProbabilityTree_GetMass(const ProbabilityTree *t);

double ProbabilityTree_ProbabilityAtIndex(const ProbabilityTree *t,
                          const int64_t index);

void ProbabilityTree_Rescale(ProbabilityTree *tree);

void ProbabilityTree_KLProject(ProbabilityTree *tree,
               const int64_t index,
               double p_new);

const int64_t ProbabilityTree_SampleIndex(ProbabilityTree *tree);

#endif  // COORDDESCENT_CODE_PROBABILITY_TREE_H_
