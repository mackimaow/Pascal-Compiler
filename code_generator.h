#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include <stdio.h>
#include "tree.h"
#include "symboltable.h"

void generateCode(FILE * fileToWrite, Tree * tree, SymbolTable * symbolTable);

#endif // CODE_GENERATOR_H