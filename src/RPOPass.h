#ifndef RPOPASS_H
#define RPOPASS_H

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"

#include <utility>
#include <vector>

struct RPOResult {
    using BlockNode = llvm::BasicBlock*;
    using BlockEdge = std::pair<BlockNode, BlockNode>;

    std::vector<BlockNode> rpo_order;
    std::vector<BlockEdge> back_edges;
};

RPOResult RPOCompute(llvm::Function& f);

#endif
