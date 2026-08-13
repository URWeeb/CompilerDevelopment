#ifndef INSTCOUNTPASS_H
#define INSTCOUNTPASS_H

#include "llvm/IR/Function.h"

#include <map>
#include <string>

/* Функция, которая считает количество инструкций каждого 
 * вида (по имени опкода) во всей функции.
 */
std::map<std::string, unsigned> ComputeInstCounts(llvm::Function& func);

#endif

