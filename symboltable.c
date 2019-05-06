#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "hashtable.h"
#include "utils.h"
#include "object.h"
#include "parsetree.h"
#include "lexconstants.h"
#include "symboltable.h"


#define TABLE_SIZE	211
#define EOS		'\0'

const int READ_PROCEDURE_ID = -1;
const int WRITE_PROCEDURE_ID = -2;

ListPrintProperties symbolTablePrintProperties = {"<--------SymbolTable_Top-------->\n", "\n", "\n<--------SymbolTable_Bot-------->\n"};
/* ----------------------------------------------------------------------------- 
 * hashpjw
 * Peter J. Weinberger's hash function 
 * Source: Aho, Sethi, and Ullman, "Compilers", Addison-Wesley, 1986 (page 436).
 */
static int hashpjw( void* value ) {
	char *s = (char*)value; 
	char *p; 
	unsigned h = 0, g; 
	
	for ( p = s; *p != EOS; p++ ) { 
		h = (h << 4) + (*p); 
		if ( g = h & 0xf0000000 ) { 
			h = h ^ ( g >> 24 ); 
			h = h ^ g; 
		} 
	} 
	return h % TABLE_SIZE; 
}

//  SCOPE_TYPE
static char * symbolTableScopeToString (ObjectType * type, void* value);
static void destroySymbolTableScope(ObjectType * type, void* value);

//  STRING_TYPE
static char * stringToString (ObjectType * type, void* value);
static int compareString(ObjectType * type, void* value1, void* value2);
static void destroyString (ObjectType * type, void* value);

//  VARIABLE_ATTR_TYPE
static char * variableAttrToString(ObjectType * type, void* value);
static void destroyVariableAttr(ObjectType * type, void* value);


static ObjectType * SCOPE_TYPE;
static ObjectType * STRING_TYPE;
static ObjectType * VARIABLE_ATTR_TYPE;

// Util Functions

static bool searchAllForEach(ObjectType * objectType, int index, void * element, void** parameters, ForEachOptions* options) {
	char* identifier = (char*) parameters[0];
	SearchResult * searchResult = (SearchResult*) parameters[1];
	SymbolTableScope * tableScope = (SymbolTableScope *) element;
	VariableAttr * variableAttr = ( VariableAttr * ) hashTableGet(tableScope->hashTable, identifier);
	if(variableAttr) {
		searchResult->scopeName = tableScope->name;
		searchResult->searchDepth = index;
		searchResult->foundResult = true;
		searchResult->attributes = variableAttr;
		return true;
	}
	return false;
}

static bool getScopeStringForEach(ObjectType * objectType, int index, void * element, void** parameters, ForEachOptions* options) {
	LinkedList * linkedList = (LinkedList *) parameters[0];
	SymbolTableScope * tableScope = (SymbolTableScope *) element;
	linkedListPush(linkedList, objectTypeToString(&STRING_OBJECT, tableScope->name));
	return false;
}


static void createSpecialProcedure(SymbolTable * symbolTable, char * procedureName, int procedureID){
	TreeValue * specialValue = parseTreeInitTreeValue(procedureID, procedureName, 0, 0);
	Tree * tree = parseTreeInit(specialValue, 0);
	symbolTablePut(symbolTable, tree, tree);
}


// Definitions
SymbolTable * symbolTableInit() {
	if(!SCOPE_TYPE)
		SCOPE_TYPE = objectTypeInit(symbolTableScopeToString, 0, destroySymbolTableScope);
	if(!STRING_TYPE)
		STRING_TYPE = objectTypeInit(stringToString, compareString, destroyString);
	if(!VARIABLE_ATTR_TYPE)
		VARIABLE_ATTR_TYPE = objectTypeInit(variableAttrToString, 0, destroyVariableAttr);
	SymbolTable * symbolTable = linkedListInitWithPrintProperties(SCOPE_TYPE, &symbolTablePrintProperties);
	
	SymbolTableScope * symbolTableScope = malloc(sizeof(SymbolTableScope));
	symbolTableScope->name = copyString("PASCAL");
	symbolTableScope->hashTable = hashTableInit(TABLE_SIZE, hashpjw, STRING_TYPE, VARIABLE_ATTR_TYPE);
	linkedListPush(symbolTable, symbolTableScope);

	createSpecialProcedure(symbolTable, "read", READ_PROCEDURE_ID);
	createSpecialProcedure(symbolTable, "write", WRITE_PROCEDURE_ID);

	return symbolTable;
}

SymbolTableScope * symbolTablePeakScope(SymbolTable * symbolTable) {
	return (SymbolTableScope *) linkedListPeak(symbolTable);
}


bool symbolTablePut(SymbolTable * symbolTable, Tree * variableValue, Tree * variableType) {
	int size = linkedListGetSize(symbolTable);
	if(size != 0) {
		SymbolTableScope * tableScope = (SymbolTableScope * ) linkedListPeak(symbolTable);
		HashTable * hashTable = tableScope->hashTable;
		VariableAttr * attributes = (VariableAttr *) malloc(sizeof(VariableAttr));
		attributes->variableValue = variableValue;
		attributes->variableType = variableType;
		attributes->scope = 0;
		char * variableName = parseTreeGetAttribute(variableValue);
		return hashTablePut(hashTable, variableName, attributes);
	}
	return false;
}

void symbolTableRemove(SymbolTable * symbolTable,  char * variableName) {
	SymbolTableScope * tableScope = (SymbolTableScope * ) linkedListPeak(symbolTable);
	HashTable * hashTable = tableScope->hashTable;
	hashTableRemove(hashTable, variableName);
}


bool symbolTableCreateScope(SymbolTable * symbolTable, Tree * properScopeLocation) {
	char * name = parseTreeGetAttribute(properScopeLocation);
	int type = parseTreeGetType(properScopeLocation);

	SymbolTableScope * symbolTableScope = malloc(sizeof(SymbolTableScope));
	symbolTableScope->name 					= name;
	symbolTableScope->scopeLocation 		= properScopeLocation;
	symbolTableScope->maxNumberOfTempRegs 	= 0;
	symbolTableScope->numberOfWhileLoops 	= 0;
	symbolTableScope->numberOfIfStatements 	= 0;
	symbolTableScope->numberOfForLoops 		= 0;
	symbolTableScope->hashTable = hashTableInit(TABLE_SIZE, hashpjw, STRING_TYPE, VARIABLE_ATTR_TYPE);

	VariableAttr * attributes = (VariableAttr *) malloc(sizeof(VariableAttr));
	attributes->variableValue 	= properScopeLocation;
	attributes->variableType 	= properScopeLocation;
	attributes->scope 	= symbolTableScope;

	SymbolTableScope * tableScope = (SymbolTableScope * ) linkedListPeak(symbolTable);
	HashTable * hashTable = tableScope->hashTable;
	return hashTablePut(hashTable, name, attributes);
}

void symbolTablePushScope(SymbolTable * symbolTable, char* variableName) {
	SymbolTableScope * tableScope = (SymbolTableScope * ) linkedListPeak(symbolTable);
	HashTable * hashTable = tableScope->hashTable;

	VariableAttr * attributes = (VariableAttr *) hashTableGet(hashTable, variableName);
	SymbolTableScope * symbolTableScope = attributes->scope;
	linkedListPush(symbolTable, symbolTableScope);
}

void symbolTablePopScope(SymbolTable * symbolTable) {
	linkedListPop(symbolTable);
}


bool symbolTableSearchScope(SymbolTable * symbolTable, char* variableName, SearchResult * searchResult) {
	searchResult->searchDepth = -1;
	if(linkedListGetSize(symbolTable) > 0){ 
		SymbolTableScope * tableScope = (SymbolTableScope * ) linkedListPeak(symbolTable);
		searchResult->scopeName = tableScope->name;
		searchResult->searchDepth = 0;
		searchResult->attributes = (VariableAttr *) hashTableGet(tableScope->hashTable, variableName);
		searchResult->foundResult = searchResult->attributes? true : false;
		return searchResult->foundResult? true : false;
	}
	return false;
}

bool symbolTableSearchAll(SymbolTable * symbolTable, char* variableName, SearchResult * searchResult) {
	searchResult->searchDepth = -1;
	if(linkedListGetSize(symbolTable) > 0)
		return linkedListForEach(symbolTable, searchAllForEach, 2, variableName, searchResult);
	return false;
}

char * symbolTableGetScopeName(SymbolTable * symbolTable) {
	SymbolTableScope * tableScope = (SymbolTableScope * ) linkedListPeak(symbolTable);
	return tableScope->name;
}






void symbolTableIncrWhile(SymbolTable * symbolTable) {
	SymbolTableScope * tableScope = (SymbolTableScope * ) linkedListPeak(symbolTable);
	tableScope->numberOfWhileLoops++;
}

void symbolTableIncrFor(SymbolTable * symbolTable) {
	SymbolTableScope * tableScope = (SymbolTableScope * ) linkedListPeak(symbolTable);
	tableScope->numberOfForLoops++;
}

void symbolTableIncrIf(SymbolTable * symbolTable) {
	SymbolTableScope * tableScope = (SymbolTableScope * ) linkedListPeak(symbolTable);
	tableScope->numberOfIfStatements++;
}

void symbolTableUpdateTempRegs(SymbolTable * symbolTable, int newValue) {
	SymbolTableScope * tableScope = (SymbolTableScope * ) linkedListPeak(symbolTable);
	if(newValue > tableScope->maxNumberOfTempRegs)
		tableScope->maxNumberOfTempRegs = newValue;
}

int symbolTableGetNumWhile(SymbolTable * symbolTable) {
	SymbolTableScope * tableScope = (SymbolTableScope * ) linkedListPeak(symbolTable);
	return tableScope->numberOfWhileLoops;
}

int symbolTableGetNumFor(SymbolTable * symbolTable) {
	SymbolTableScope * tableScope = (SymbolTableScope * ) linkedListPeak(symbolTable);
	return tableScope->numberOfForLoops;
}

int symbolTableGetNumIf(SymbolTable * symbolTable) {
	SymbolTableScope * tableScope = (SymbolTableScope * ) linkedListPeak(symbolTable);
	return tableScope->numberOfIfStatements;
}


int symbolTablegetNumTempRegs(SymbolTable * symbolTable) {
	SymbolTableScope * tableScope = (SymbolTableScope * ) linkedListPeak(symbolTable);
	return tableScope->maxNumberOfTempRegs;
}







char* symbolTableScopeTraceString(SymbolTable * symbolTable, ListPrintProperties * listPrintProperties) {
	int tableSize = linkedListGetSize(symbolTable);
	if(tableSize > 0) {
		LinkedList * stringList = linkedListInitWithPrintProperties(&STRING_OBJECT, listPrintProperties);
		linkedListForEach(symbolTable, getScopeStringForEach, 1, stringList);
		linkedListPop(stringList);
		char * temp = linkedListToString(stringList);
		linkedListDestroy(stringList);
		return temp;
	}
	return 0;
}

int symbolTableGetScopeDepth(SymbolTable * symbolTable) {
	return linkedListGetSize(symbolTable);
}

char* symbolTableScopeName(SymbolTable * symbolTable) {
	if(linkedListGetSize(symbolTable) > 0){ 
		SymbolTableScope * tableScope = (SymbolTableScope * ) linkedListPeak(symbolTable);
		return tableScope->name;
	}
	return 0;
}


void symbolTablePrint(SymbolTable * symbolTable) {
	linkedListPrint(symbolTable);
}

char* symbolTableToString(SymbolTable * symbolTable) {
	return linkedListToString(symbolTable);
}








//  SCOPE_TYPE
static char * symbolTableScopeToString (ObjectType * type, void* value) {
	SymbolTableScope * scope = (SymbolTableScope *) value;
	int scopeNameLength = getStringSize(scope->name);
	char * firstString = "  [--";
	char * secondString = "--]  ";
	int firstStringLength = getStringSize(firstString);
	int secondStringLength = getStringSize(secondString);
	char * tableString = hashTableToString(scope->hashTable);
	int tableStringLength = getStringSize(tableString);
	int totalSize = firstStringLength + scopeNameLength + secondStringLength + tableStringLength;
	char * temp = malloc(sizeof(char) * (totalSize + 1));
	int currentIndex = 0;
	stringInsert(temp, firstString, currentIndex);
	currentIndex += firstStringLength;
	stringInsert(temp, scope->name, currentIndex);
	currentIndex += scopeNameLength;
	stringInsert(temp, secondString, currentIndex);
	currentIndex += secondStringLength;
	stringInsert(temp, tableString, currentIndex);
	*(temp+totalSize) = '\0';
	free(tableString);
	return temp;
}

static void destroySymbolTableScope(ObjectType * type, void* value) {
	// NOT  NECCESSARY
	// SymbolTableScope * scope = (SymbolTableScope *) value;
	// hashTableDestroy(scope->hashTable);
	// free(scope->name);
	// free(scope);
}

//  STRING_TYPE
static char * stringToString (ObjectType * type, void* value) {
	char * str = (char *) value;
	int stringSize = getStringSize(str);
	char * temp = malloc(sizeof(char) * (stringSize + 1));
	stringInsert(temp, str, 0);
	*(temp+stringSize) = '\0';
	return temp;
}

static int compareString(ObjectType * type, void* value1, void* value2) {
	return strcmp((char*)value1, (char*)value2);
}

static void destroyString (ObjectType * type, void* value) {
	char * str = (char *) value;
	// free(str);
}

//  VARIABLE_ATTR_TYPE
static char * variableAttrToString(ObjectType * objectType, void* value) {
	VariableAttr * variableAttr = (VariableAttr*) value;
	int type = parseTreeGetType(variableAttr->variableValue);
	char * str = lexConstantToString(type);
	int stringSize = getStringSize(str);
	char * temp = malloc(sizeof(char) * (stringSize + 1));
	stringInsert(temp, str, 0);
	*(temp+stringSize) = '\0';
	return temp;
}

static void destroyVariableAttr(ObjectType * objectType, void* value) {
	// NOT NECCESSARY
	// VariableAttr * variableAttr = (VariableAttr*) value;
	// free(variableAttr->type);
	// free(variableAttr);
}