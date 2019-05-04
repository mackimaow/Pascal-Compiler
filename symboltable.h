#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdbool.h>
#include "linkedlist.h"
#include "hashtable.h"

#define SymbolTable LinkedList


typedef struct SymbolTableScope {
	char* name;
	HashTable * hashTable;
	int maxNumberOfTempRegs;
} SymbolTableScope;

typedef struct VariableAttr {
	char* type;
} VariableAttr;

typedef struct SearchResult {
	char* scopeName;
	int searchDepth;
	bool foundResult;
	VariableAttr attributes;
} SearchResult;

SymbolTable * symbolTableInit();
bool symbolTablePut(SymbolTable * symbolTable, char* variableName, VariableAttr * variableAttr);
void symbolTableSearchScope(SymbolTable * symbolTable, char* variableName, SearchResult * searchResult);
void symbolTableSearchAll(SymbolTable * symbolTable, char* variableName, SearchResult * searchResult);
void symbolTablePushScope(SymbolTable * symbolTable, char* scopeName);
void symbolTablePopScope(SymbolTable * symbolTable);
char* symbolTableScopeTraceString(SymbolTable * symbolTable, ListPrintProperties * listPrintProperties);
int  symbolTableGetScopeDepth(SymbolTable * symbolTable);
void symbolTablePrint(SymbolTable * symbolTable);
char* symbolTableToString(SymbolTable * symbolTable);

#endif // SYMBOL_TABLE_H


