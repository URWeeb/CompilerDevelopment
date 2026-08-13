#include "InstCountPass.h"

#include "llvm/IR/Instruction.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Plugins/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

std::map<std::string, unsigned> ComputeInstCounts(llvm::Function& func) {
    std::map<std::string, unsigned> counts;

    for (auto& basic_block : func) {
        for (auto& instruction : basic_block) {
            ++counts[std::string(instruction.getOpcodeName())];
        }
    }

    return counts;
}

namespace {
    void VisitFunction(llvm::Function& func) {
        auto counts = ComputeInstCounts(func);

        llvm::errs() << "Function " << func.getName() << "():\n";

        for (auto& [name, count] : counts) {
            llvm::errs() << "  " << name << ": " << count << "\n";
        }
    }

    struct InstCountPass : llvm::PassInfoMixin<InstCountPass> {
        llvm::PreservedAnalyses run(llvm::Function& func,
                                   llvm::FunctionAnalysisManager& a_manager) {
            if (func.isDeclaration()) {
                return llvm::PreservedAnalyses::all();
            }

            VisitFunction(func);

            return llvm::PreservedAnalyses::all();
        }


        static bool isRequired(void) {
            return true;
        }
    };

    bool CallBackForPipelineParser(
                                    llvm::StringRef name,
                                    llvm::FunctionPassManager& func_manager,
                                    llvm::ArrayRef<llvm::PassBuilder::PipelineElement>
                                   ) {
        if (name == "inst-count-pass") {
            func_manager.addPass(InstCountPass());
            return true;
        } else {
            return false;
        }
    }

    void CallBackForPassBuilder(llvm::PassBuilder& pass_builder) {
        pass_builder.registerPipelineParsingCallback(&CallBackForPipelineParser);
    }

    llvm::PassPluginLibraryInfo getInstCountPassPluginInfo(void) {
        uint32_t     APIversion    = LLVM_PLUGIN_API_VERSION;
        const char * PluginName    = "InstCountPass";
        const char * PluginVersion = LLVM_VERSION_STRING;

        llvm::PassPluginLibraryInfo info =
        {
            APIversion,
            PluginName,
            PluginVersion,
            CallBackForPassBuilder
        };

        return (info);
    }
}


/*
 * Интерфейс, который нужен, чтобы гарантировать, что "opt" распознает наш проход.
 * "-passes=inst-count-pass"
 */
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return (getInstCountPassPluginInfo());
}
