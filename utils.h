#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include "object.h"


// string hashtable stuff  
#define TABLE_SIZE	211
#define EOS		'\0'

int hashpjw( void* value ); // hash function on string types


// List utils

typedef struct ForEachOptions {
	bool remove;
	void* insert;
} ForEachOptions;

typedef struct ListPrintProperties {
	char * startString;
	char * separatorString;
	char * endString;
} ListPrintProperties;

typedef struct MapPrintProperties {
	char * startStringPair;
	char * separatorStringPair;
	char * endStringPair;
	ListPrintProperties * listPrintProperties;
} MapPrintProperties;

typedef struct TreePrintProperties {
	char * left;
	char * right;
	char * vertical;
	char * horizontal;
} TreePrintProperties;

extern ListPrintProperties defaultPrintProperties;
extern MapPrintProperties defaultMapPrintProperties;
extern TreePrintProperties defaultTreePrintProperties;

void resetOptions (ForEachOptions * options);

typedef bool ForEach (ObjectType * objectType, int index, void * element, void** parameters, ForEachOptions* options);

// Misc

typedef struct Pair {
	void* first;
	void* second;
} Pair;

typedef struct Single {
	void* element;
} Single;


int stringToInt(char * value);
char * intToString(int integer);
char * copyString(char * stringValue);
int getStringSize(char * stringValue);
void stringInsert(char * totalString, char * insertString, int startIndex);
char * stringTakeLast(char * totalString, int startInclusive);
char * stringTakeSubstring(char * totalString, int startInclusive, int endExclusize);


#endif // UTILS_H