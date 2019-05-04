#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "hashtable.h"
#include "utils.h"
#include "object.h"




struct HashTable {
	int size;
	int capacity;
	HashFunction * hashFunction;
	ObjectType * pairType;
	ListPrintProperties * bucketPrintProperties;
	LinkedList ** buckets;
};

// Utils

static char * pairToString(ObjectType * pairType, void* value) {
	ObjectType * keyType = objectTypeGetNestedType(pairType, 0);
	ObjectType * elementType = objectTypeGetNestedType(pairType, 1);
	Pair* pair = (Pair*)value;
	MapPrintProperties * printProperties = objectTypeGetProperty(pairType, 0);

	int startStringSize 	= getStringSize(printProperties->startStringPair);
	int separatorStringSize = getStringSize(printProperties->separatorStringPair);
	int endStringSize		= getStringSize(printProperties->endStringPair);
	char * firstString = objectTypeToString(keyType, pair->first);
	char * lastString  = objectTypeToString(elementType, pair->second); 
	int firstStringSize = getStringSize(firstString);
	int lastStringSize = getStringSize(lastString);
	int totalSize = startStringSize + firstStringSize + separatorStringSize + lastStringSize + endStringSize;
	char * temp = malloc(sizeof(char) * (totalSize + 1));
	int currentIndex = 0;
	stringInsert(temp, printProperties->startStringPair, currentIndex);
	currentIndex += startStringSize;
	stringInsert(temp, firstString, currentIndex);
	currentIndex += firstStringSize;
	stringInsert(temp, printProperties->separatorStringPair, currentIndex);
	currentIndex += separatorStringSize;
	stringInsert(temp, lastString,  currentIndex);
	currentIndex += lastStringSize;
	stringInsert(temp, printProperties->endStringPair, currentIndex);
	*(temp+totalSize) = '\0';

	free(firstString);
	free(lastString);
	return temp;
}

static void pairDestructor (ObjectType * pairType, void* value) {
	ObjectType * keyType = objectTypeGetNestedType(pairType, 0);
	ObjectType * elementType = objectTypeGetNestedType(pairType, 1);
	Pair* pair = (Pair*)value;
	objectTypeDestroyValue(keyType, pair->first);
	objectTypeDestroyValue(elementType, pair->second);
	free(pair);
}

static bool findPlaceKeyElemForEach(ObjectType * objectType, int index, void * element, void** parameters, ForEachOptions* options) {
	Pair * original = (Pair *) element;
	Pair * toPlace = (Pair *)parameters[0];

	ObjectType * keyType = objectTypeGetNestedType(objectType, 0);
	int compareValue = objectTypeCompare(keyType, original->first, toPlace->first);
	
	if(compareValue == 0) {
		options->remove = true;
		options->insert = toPlace;
		return true;
	}
	return false;
}

static bool getElementForEach(ObjectType * objectType, int index, void * element, void** parameters, ForEachOptions* options) {
	Pair * original = (Pair *) element;
	void * key = parameters[0];
	Single * single = (Single *)parameters[1];

	ObjectType * keyType = objectTypeGetNestedType(objectType, 0);
	int compareValue = objectTypeCompare(keyType, original->first, key);

	if( compareValue == 0 ) {
		single->element = original->second;
		return true;
	}
	return false;
}

static bool removeForEach(ObjectType * objectType, int index, void * element, void** parameters, ForEachOptions* options) {
	Pair * original = (Pair *) element;
	void * key = parameters[0];

	ObjectType * keyType = objectTypeGetNestedType(objectType, 0);
	int compareValue = objectTypeCompare(keyType, original->first, key);

	if( compareValue == 0 ) {
		options->remove = true;
		return true;
	}
	return false;
}

// actual functions

HashTable* hashTableInit( int capacity, HashFunction * hashFunction, ObjectType * keyType, ObjectType * elementType) {
	return hashTableInitWithPrintProperties( capacity,  hashFunction,  keyType, elementType, &defaultMapPrintProperties);
}


HashTable* hashTableInitWithPrintProperties( int capacity, HashFunction * hashFunction, ObjectType * keyType, ObjectType * elementType, MapPrintProperties * printProperties) {
	HashTable * table = malloc(sizeof(HashTable));
	table->size = 0;
	table->capacity = capacity;
	table->hashFunction = hashFunction;
	table->buckets = malloc(sizeof(HashTable *) * capacity);

	table->pairType = objectTypePropertiesNestedInit(pairToString, 0, pairDestructor, 2, 1, keyType, elementType, printProperties);

	table->bucketPrintProperties = malloc(sizeof(ListPrintProperties));
	table->bucketPrintProperties->startString = "";
	table->bucketPrintProperties->separatorString = printProperties->listPrintProperties->separatorString;
	table->bucketPrintProperties->endString   = "";

	for(int i = 0; i < capacity; i++)
		*(table->buckets + i) = 0;
	return table;
}

void hashTableDestroy(HashTable * hashTable) {
	for(int i = 0; i < hashTable->capacity; i++) {
		LinkedList * bucket = hashTable->buckets[i];
		if(bucket)
			linkedListDestroy(bucket);
	}
	free(hashTable->buckets);
	objectTypeDestroyTypeOnly(hashTable->pairType);
	free(hashTable->bucketPrintProperties);
	free(hashTable);
}

bool hashTablePut(HashTable * hashTable, void* key, void* element) {
	int index = hashTable->hashFunction(key);
	LinkedList * bucket = hashTable->buckets[index];

	if(!bucket) {
		bucket = linkedListInitWithPrintProperties(hashTable->pairType, hashTable->bucketPrintProperties);
		hashTable->buckets[index] = bucket;
	}

	Pair * pair = malloc(sizeof(Pair));
	pair->first = key;
	pair->second = element;

	bool stopShort = linkedListForEach(bucket, findPlaceKeyElemForEach, 1, pair);
	if(!stopShort) {
		linkedListPush(bucket, pair);
		hashTable->size++;
		return false;
	} else {
		return true;
	}
}

void* hashTableGet(HashTable * hashTable, void* key) {
	int index = hashTable->hashFunction(key);
	LinkedList * bucket = hashTable->buckets[index];

	if(bucket) {
		Single single = { 0 };
		bool stopShort = linkedListForEach(bucket, getElementForEach, 2, key, &single);
		if(stopShort)
			return single.element;
	}
	return 0;
}

void hashTableRemove(HashTable * hashTable, void* key) {
	int index = hashTable->hashFunction(key);
	LinkedList * bucket = hashTable->buckets[index];

	if(bucket) {
		bool stopShort = linkedListForEach(bucket, removeForEach, 1, key);
		if(stopShort)
			hashTable->size--;
	}
}

void hashTableClear(HashTable * hashTable) {
	for(int i = 0; i < hashTable->capacity; i++) {
		LinkedList * bucket = hashTable->buckets[i];
		if(bucket)
			linkedListClear(bucket);
	}
	hashTable->size = 0;
}

int  hashTableSize(HashTable * hashTable) {
	return hashTable->size;
}

int  hashTableCapacity(HashTable * hashTable) {
	return hashTable->capacity;
}

void hashTablePrint(HashTable * hashTable) {
	char * temp = hashTableToString(hashTable);
	printf("%s", temp);
	free(temp);
}


char* hashTableToString(HashTable * hashTable) {
	MapPrintProperties * printProperties = (MapPrintProperties *) objectTypeGetProperty(hashTable->pairType, 0);
	LinkedList * bucketStrings = linkedListInitWithPrintProperties(&STRING_OBJECT, printProperties->listPrintProperties);

	for(int i = 0; i < hashTable->capacity; i++) {
		LinkedList * bucket = hashTable->buckets[i];

		if(bucket && linkedListGetSize(bucket) > 0) {
			char * stringValue = linkedListToString(bucket);
			linkedListPush(bucketStrings, linkedListToString(bucket));
		}
	}
	char * temp = linkedListToString(bucketStrings);
	linkedListDestroy(bucketStrings);

	return temp;	
}