#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdbool.h>
#include "linkedlist.h"
#include "tree.h"
#include "hashtable.h"

#define SymbolTable LinkedList

#define READ_PROCEDURE_ID -1
#define WRITE_PROCEDURE_ID -2

typedef struct SymbolTableScope {
	char* name;
	HashTable * hashTable;
	Tree * scopeLocation;
	int maxNumberOfTempRegs;
	int numberOfWhileLoops;
	int numberOfIfStatements;
	int numberOfForLoops;
} SymbolTableScope;

typedef struct VariableAttr {
	Tree * variableValue;
	Tree * variableType;
	SymbolTableScope * scope;
} VariableAttr;

typedef struct SearchResult {
	char* scopeName;
	int searchDepth;
	bool foundResult;
	VariableAttr * attributes;
} SearchResult;

SymbolTable * symbolTableInit();
bool symbolTablePut(SymbolTable * symbolTable,  Tree * variableValue, Tree * variableType);
void symbolTableRemove(SymbolTable * symbolTable,  char * variableName);
SymbolTableScope * symbolTablePeakScope(SymbolTable * symbolTable);

bool symbolTableCreateScope(SymbolTable * symbolTable, Tree * properScopeLocation);
void symbolTablePushScope(SymbolTable * symbolTable, char* variableName);
void symbolTablePopScope(SymbolTable * symbolTable);

bool symbolTableSearchScope(SymbolTable * symbolTable, char* variableName, SearchResult * searchResult);
bool symbolTableSearchAll(SymbolTable * symbolTable, char* variableName, SearchResult * searchResult);
char * symbolTableGetScopeName(SymbolTable * symbolTable);

void symbolTableIncrWhile(SymbolTable * symbolTable);
void symbolTableIncrFor(SymbolTable * symbolTable);
void symbolTableIncrIf(SymbolTable * symbolTable);
void symbolTableUpdateTempRegs(SymbolTable * symbolTable, int newValue);


int symbolTableGetNumWhile(SymbolTable * symbolTable);
int symbolTableGetNumFor(SymbolTable * symbolTable);
int symbolTableGetNumIf(SymbolTable * symbolTable);
int symbolTablegetNumTempRegs(SymbolTable * symbolTable);


char* symbolTableScopeTraceString(SymbolTable * symbolTable, ListPrintProperties * listPrintProperties);
int  symbolTableGetScopeDepth(SymbolTable * symbolTable);
void symbolTablePrint(SymbolTable * symbolTable);
char* symbolTableToString(SymbolTable * symbolTable);

#endif // SYMBOL_TABLE_H


