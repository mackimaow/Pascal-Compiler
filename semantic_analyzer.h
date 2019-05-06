#ifndef SEMANTIC_ANALYIZER_H
#define SEMANTIC_ANALYIZER_H

#include "symboltable.h"
#include "tree.h"

SymbolTable * analyzeSemantics(Tree * tree);
	
#endif // SEMANTIC_ANALYIZER_H