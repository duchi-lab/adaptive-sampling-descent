# Simple script for testing binary tree for adaptive sampling.
# Runs SVM on a small binary classification example.

include("SimpleTree.jl");

# Set seed
srand(12345);

# =================== Generate data ====================
nn = 10;
dd = 5;

Xtrain = randn(nn, dd);
ytrain = rand([-1.0, 1.0], nn);

# compute approximate lipschitz constant
lip = 0;
for ii = 1:nn
  lip = max(lip, norm(Xtrain[ii, :]));
end

# =================== Set paramters ====================
# default blocks are [[1], ..., [nn]]
pmin = .1 / nn;
shrink_factor = .1;

init_step_prob = .001;
init_step_point = .1;

init_prob = ones(nn) / nn;
init_point = zeros(dd);

# regularization
ball_type = 2.0; # 2-norm
radius = 10;

num_iter = 10 * nn;

# =================== Optimization =====================

# ----------------------------------------------------------

# initialize
upper = shrink_factor * lip^2 / (nn * pmin)^2;
prob = init_prob;
point = init_point;
g = zeros(dd);

tree = CreateTree(nn, pmin);

losses = zeros(num_iter);

for iter = 1:num_iter
  # sample
  ind = SampleIndex(tree);
  p_ind = ProbabilityAtIndex(tree, ind);

  # compute stochastic gradient for the hinge loss at (ind)-th datapoint
  g = - ytrain[ind] * Xtrain[ind, :] * (1 - ytrain[ind] * dot(Xtrain[ind, :], point) > 0);

  # rescale gradient (importance sampling)
  g /= nn * p_ind;

  # sgd update
  point -= (init_step_point / sqrt(iter)) * g;

  # project to feasible region
  norm_point = norm(point, ball_type);
  if (norm_point > radius)
    point *= radius / norm_point;
  end

  # update and project sampling probability
  bandit_grad = max(upper - norm(g)^2, 0);
  p_new = p_ind * exp(-(init_step_prob / (p_ind)) * bandit_grad);
  prob = KLProject(tree, ind, p_new);

  # evaluate objective
  losses[iter] = (1/nn) * sum(max(1 - (spdiagm(ytrain) * Xtrain) * point, 0));

  # print
  @printf("Iteration %d of %d, obj = %f\n", iter, num_iter, losses[iter]);
end


# # CVX solution for comparison
# using Convex, Gurobi;
# point = Variable(dd);
# obj = (1/nn) * sum(max(1 - (spdiagm(ytrain) * Xtrain) * point, 0));
# model = minimize(obj);
# model.constraints += (norm(point, 2) <= radius);
# solve!(model);
# optval = model.optval;

