#ifndef RPOPASS_H
#define RPOPASS_H

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"

#include <utility>
#include <vector>
/*
 * Структура для хранения результата обхода CFG.
 *
 * - Алиасы:
 * -- BlockNode - указатель на llvm::BasicBlock;
 * -- BackEdge - std::pair двух BlockNode, будет использоваться для
 *    хранения обратных дуг;
 *
 * - Поля
 * -- std::vector<BlockNode> rpo_order - вектор базовых блоков, расположенных в
 *    в порядке RPO;
 * -- std::vector<BackEdge> back_edges - вектор найденных обратных дуг;
 */
struct RPOResult {
    using BlockNode = llvm::BasicBlock*;
    using BackEdge = std::pair<BlockNode, BlockNode>;

    std::vector<BlockNode> rpo_order;
    std::vector<BackEdge> back_edges;
};

/*
 * Функция для обхода CFG итеративным DFS для поиска 
 * обратных дуг и записи нод в RPO-порядке
 * (аналогична графовой задачи, выданной ранее)
 */
RPOResult RPOCompute(llvm::Function& f);

#endif
