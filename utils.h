#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include "object.h"


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

char * copyString(char * stringValue);
int getStringSize(char * stringValue);
void stringInsert(char * totalString, char * insertString, int startIndex);

#endif // UTILS_H