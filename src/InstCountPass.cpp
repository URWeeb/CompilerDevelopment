#include "InstCountPass.h"

#include "llvm/IR/Instruction.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Plugins/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

/*
 * Имплементация функции ComputeInstCounts, описанной ранее в соответ-
 * ствующем header-файле
 */
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
    /*
     * Функция, которая выводит кол-во инструкций каждого вида
     * (по имени опкода) для функции func в формате "<опкод>: <count>".
     * Вывод отсортирован по именам опкодов
     */
    void VisitFunction(llvm::Function& func) {
        auto counts = ComputeInstCounts(func);

        llvm::errs() << "Function " << func.getName() << "():\n";

        for (auto& [name, count] : counts) {
            llvm::errs() << "  " << name << ": " << count << "\n";
        }
    }
    
    /*
     * Структура, имплементирующая проход нового pass manager'а LLVM
     */
    struct InstCountPass : llvm::PassInfoMixin<InstCountPass> {
        /*
         * Точка входа прохода для конкретной функции func.
         */
        llvm::PreservedAnalyses run(llvm::Function& func,
                                   llvm::FunctionAnalysisManager& a_manager) {
            if (func.isDeclaration()) {
                return llvm::PreservedAnalyses::all();
            }

            VisitFunction(func);

            return llvm::PreservedAnalyses::all();
        }

        /*
         * Метод, который требует запуск прохода даже для функций 
         * с атрибутом optnone
         */
        static bool isRequired(void) {
            return true;
        }
    };

    /*
     * Callback парсера текстового пайплайна
     */
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

    /*
     * Функция, которая регистрирует CallBackForPipelineParser в pass_builder
     */
    void CallBackForPassBuilder(llvm::PassBuilder& pass_builder) {
        pass_builder.registerPipelineParsingCallback(&CallBackForPipelineParser);
    }

    /*
     * Функция, которая собирает структуру PassPluginLibraryInfo: 
     * версия API плагинов, имя, версия LLVM, callback регистрации
     */
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
