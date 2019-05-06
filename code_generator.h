#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "tree.h"
#include "symboltable.h"

char * generateCode(Tree * tree, SymbolTable * symbolTable);

#endif // CODE_GENERATOR_H