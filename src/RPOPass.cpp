#include "RPOPass.h"

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/CFG.h"
#include "llvm/Passes/PassBuilder.h"
/* Поскольку я скомпилировал LLVM 24, здесь указан такой путь до header-файла
*  Если вы используете LLVM 15, замените Plugins на Passes
*/
#include "llvm/Plugins/PassPlugin.h" // LLVM 15: llvm/Passes/PassPlugin.h; LLVM 24: llvm/Plugins/PassPlugin.h
#include "llvm/Support/raw_ostream.h"

/* 
 * Набор алиасов, часть из которых взята из соответствующего header-файла
 * Нужны исключительно в качестве синтаксического сахара
 * (что в прочем очевидно :/)
 */
using BlockNode = llvm::BasicBlock*;
using BlockEdge = std::pair<BlockNode, BlockNode>;
using StackEntry = std::pair<BlockNode, llvm::succ_iterator>;

/*
 * Реализация функции RPOCompute, описанной в header-файле RPOPass.h
 */
RPOResult RPOCompute(llvm::Function& func) {
    RPOResult result;
    
    llvm::SmallVector<StackEntry, 32> stack;
    llvm::DenseSet<const BlockNode> visited;
    llvm::DenseSet<const BlockNode> on_stack;
    llvm::SmallVector<BlockNode, 32> post_order;

    BlockNode entry = &func.getEntryBlock();
    visited.insert(entry);
    on_stack.insert(entry);
    stack.emplace_back(entry, llvm::succ_begin(entry));

    while (!stack.empty()) {
        auto& [basic_block, iter] = stack.back();

        if (iter == llvm::succ_end(basic_block)) {
            post_order.push_back(basic_block);
            on_stack.erase(basic_block);
            stack.pop_back();
            
            return;
        }

        Node succ = *iter;
        ++iter;

        if (!visited.count(succ)) {
            visited.insert(succ);
            on_stack.insert(succ);
            stack.emplace_back(succ, llvm::succ_begin(succ));
        } else if (on_stack.count(succ)) {
            result.back_edges.emplace_back(basic_block, succ);
        }
    }

    result.rpo_order.assign(post_order.rbegin(), post_order.rend());

    return result;
}

namespace {

    void VisitFunction(llvm::Function& func) {
        llvm::DenseMap<const BlockNode, unsigned> basic_block_ids;
        unsigned following_id = 0;

        for (auto& basic_block : func) {
            basic_block_ids[&basic_block] = following_id;
            ++following_id;
        }

        RPOResult result = RPOCompute(func);

        llvm::errs() << "Function name: " << func.getName() << "\n";
        llvm::errs() << "RPO order:\n";

        for (BlockNode basic_block : result.rpo_order) {
            llvm::errs() << " BB" << basic_block_ids[basic_block] << "\n";
        }

        llvm::errs() << "Back edges:\n";

        if (result.back_edges.empty()) {
            llvm::errs() << " none\n";
        } else {
            for (auto& [from, to] : result.back_edges) {
                llvm::errs() << " BB" << basic_block_ids[from] << " -> BB" <<
                    basic_block_ids[to] << "\n";
            }
        }
    }

    struct RPOPass : llvm::PassInfoMixin<RPOPass> {
        llvm::PreserveAnalyses run(llvm::Function& func, 
                                   llvmd::FunctionAnalysisManager& a_manager) {
            if (func.isDeclaration()) {
                return (llvm::PreservedAnalyses::all());
            }

            VisitFunction(func);

            return (llvm::PreservedAnalyses::all());
        }

        static bool isRequired(void) {
            return true;
        }
    };

    bool CallBackForPipelineParser(
        llvm::StringRef name,
        llvm::FunctionPassManager& fp_manager,
        llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
        if (name == "rpo-pass") {
            fp_manager.addPass(RPOPass());
            return true;
        } else {
            return false;
        }
    }

    void CallBackForPassBuilder(llvm::PassBuilder& pass_builder) {
        pass_builder.registerPipelineParsingCallback(&CallBackForPipelineParser);
    }

    llvm::PassPluginLibraryInfo getRPOPassPluginInfo(void) {
        uint32_t APIversion = LLVM_PLUGIN_API_VERSION;
        const char* PluginName = "RPOPass";
        const char* PluginVersion = LLVM_VERSION_STRING;

        llvm::PassPluginLibraryInfo info =
        {
            APIversion,
            PluginName,
            PluginVersion,
            CallBackForPassBuilder
        };

        return info;
    }

} /* namespace */

/*
 * Интерфейс, что будет гарантировать распознавание прохода "opt"
 * "-passes=rpo-pass"
 */
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo llvmGetPluginInfo() {
    return getRPOPassPluginInfo();
}
