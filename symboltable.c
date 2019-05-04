#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "hashtable.h"
#include "utils.h"
#include "object.h"
#include "symboltable.h"


#define TABLE_SIZE	211
#define EOS		'\0'



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
	VariableAttr* variableAttr = hashTableGet(tableScope->hashTable, identifier);
	if(variableAttr) {
		searchResult->scopeName = tableScope->name;
		searchResult->searchDepth = index;
		searchResult->foundResult = variableAttr;
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



// Definitions

SymbolTable * symbolTableInit() {
	if(!SCOPE_TYPE)
		SCOPE_TYPE = objectTypeInit(symbolTableScopeToString, 0, destroySymbolTableScope);
	if(!STRING_TYPE)
		STRING_TYPE = objectTypeInit(stringToString, compareString, destroyString);
	if(!VARIABLE_ATTR_TYPE)
		VARIABLE_ATTR_TYPE = objectTypeInit(variableAttrToString, 0, destroyVariableAttr);
	SymbolTable * symbolTable = linkedListInitWithPrintProperties(SCOPE_TYPE, &symbolTablePrintProperties);
	return symbolTable;
}

bool symbolTablePut(SymbolTable * symbolTable, char* identifier, VariableAttr * variableAttr) {
	int size = linkedListGetSize(symbolTable);
	if(size != 0) {
		SymbolTableScope * tableScope = (SymbolTableScope * ) linkedListPeak(symbolTable);
		HashTable * hashTable = tableScope->hashTable;
		return hashTablePut(hashTable, identifier, variableAttr);
	}
	return false;
}

void symbolTableSearchScope(SymbolTable * symbolTable, char* identifier, SearchResult * searchResult) {
	searchResult->searchDepth = -1;
	if(linkedListGetSize(symbolTable) > 0){ 
		SymbolTableScope * tableScope = (SymbolTableScope * ) linkedListPeak(symbolTable);
		searchResult->scopeName = tableScope->name;
		searchResult->searchDepth = 0;
		searchResult->foundResult = hashTableGet(tableScope->hashTable, identifier);
	}
}

void symbolTableSearchAll(SymbolTable * symbolTable, char* identifier, SearchResult * searchResult) {
	searchResult->searchDepth = -1;
	if(linkedListGetSize(symbolTable) > 0)
		linkedListForEach(symbolTable, searchAllForEach, 2, identifier, searchResult);
}

void symbolTablePushScope(SymbolTable * symbolTable, char* scopeName) {

	SymbolTableScope * symbolTableScope = malloc(sizeof(SymbolTableScope));
	symbolTableScope->name = scopeName;
	symbolTableScope->hashTable = hashTableInit(TABLE_SIZE, hashpjw, STRING_TYPE, VARIABLE_ATTR_TYPE);
	
	linkedListPush(symbolTable, symbolTableScope);
}

void symbolTablePopScope(SymbolTable * symbolTable) {
	linkedListPop(symbolTable);
}

char* symbolTableScopeTraceString(SymbolTable * symbolTable, ListPrintProperties * listPrintProperties) {
	int tableSize = linkedListGetSize(symbolTable);
	if(tableSize > 0) {
		LinkedList * stringList = linkedListInitWithPrintProperties(&STRING_OBJECT, listPrintProperties);
		linkedListForEach(symbolTable, getScopeStringForEach, 1, stringList);
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
	SymbolTableScope * scope = (SymbolTableScope *) value;
	hashTableDestroy(scope->hashTable);
	// free(scope->name);
	free(scope);
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
static char * variableAttrToString(ObjectType * type, void* value) {
	VariableAttr * variableAttr = (VariableAttr*) value;
	char * str = variableAttr->type;
	int stringSize = getStringSize(str);
	char * temp = malloc(sizeof(char) * (stringSize + 1));
	stringInsert(temp, str, 0);
	*(temp+stringSize) = '\0';
	return temp;
}

static void destroyVariableAttr(ObjectType * type, void* value) {
	VariableAttr * variableAttr = (VariableAttr*) value;
	// free(variableAttr->type);
	// free(variableAttr);
}