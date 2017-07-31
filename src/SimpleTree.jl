immutable TreeNode
    mass::Cdouble
    mass_left::Cdouble
    mass_right::Cdouble
    left::Ptr{TreeNode}
    right::Ptr{TreeNode}
    parent::Ptr{TreeNode}
end

immutable ProbabilityTree
    tree::Ptr{TreeNode}
    root::Ptr{TreeNode}
    length::Csize_t
    p_min::Cdouble
end

function CreateTree(n::Int64, p_min::Float64)
    return ccall((:ConstructProbabilityTree,
                  "./simple.so"),
                 Ptr{ProbabilityTree}, (Csize_t, Cdouble), n, p_min);
end

function CreateTreeWithWeights(n::Int64, p_min::Float64, p::Array{Float64})
    return ccall((:ConstructProbabilityTreeWithWeights,
                  "./simple.so"),
                 Ptr{ProbabilityTree}, (Csize_t, Cdouble, Ptr{Cdouble}), n, p_min, p);
end

function ProbabilityAtIndex(tree::Ptr{ProbabilityTree}, index::Int64)
    return ccall((:ProbabilityTree_ProbabilityAtIndex,
                  "./simple.so"),
                 Cdouble, (Ptr{ProbabilityTree}, Int64), tree, index - 1);
end

function KLProject(tree::Ptr{ProbabilityTree}, index::Int64, p_new::Float64)
    return ccall((:ProbabilityTree_KLProject,
                  "./simple.so"),
                 Void, (Ptr{ProbabilityTree}, Int64, Float64),
                 tree, index - 1, p_new);
end

function SampleIndex(tree::Ptr{ProbabilityTree})
    return ccall((:ProbabilityTree_SampleIndex,
                  "./simple.so"),
                 Int64, (Ptr{ProbabilityTree},), tree) + 1;
end
