#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "hashtable.h"
#include "utils.h"




struct HashTable {
	int size;
	int capacity;
	HashFunction * hashFunction;
	ObjectType * pairType;
	ListPrintProperties * bucketPrintProperties;
	LinkedList ** buckets;
};

// Utils

static void pairPrint(ObjectType * pairType, void* value) {
	ObjectType * keyType = objectTypeGetNestedType(pairType, 0);
	ObjectType * elementType = objectTypeGetNestedType(pairType, 1);
	Pair* pair = (Pair*)value;
	MapPrintProperties * printProperties = objectTypeGetProperty(pairType, 0);
	printf("%s", printProperties->startStringPair);
	objectTypePrintValue(keyType, pair->first);
	printf("%s", printProperties->separatorStringPair);
	objectTypePrintValue(elementType, pair->second);
	printf("%s", printProperties->endStringPair);
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

	table->pairType = objectTypePropertiesNestedInit(pairPrint, 0, pairDestructor, 2, 1, keyType, elementType, printProperties);

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

void hashTablePut(HashTable * hashTable, void* key, void* element) {
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

void printTable(HashTable * hashTable) {
	MapPrintProperties * printProperties = (MapPrintProperties *) objectTypeGetProperty(hashTable->pairType, 0);
	printf("%s", printProperties->listPrintProperties->startString);
	bool printedLastString = false;
	for(int i = 0; i < hashTable->capacity; i++) {
		LinkedList * bucket = hashTable->buckets[i];

		if(bucket && linkedListGetSize(bucket) > 0) {
			if(printedLastString) {
				printf("%s", printProperties->listPrintProperties->separatorString);
			} else{
				printedLastString = true;				
			}
			linkedListPrint(bucket);
		}
	}
	printf("%s", printProperties->listPrintProperties->endString);
}


// /* ----------------------------------------------------------------------------- 
//  * hashpjw
//  * Peter J. Weinberger's hash function 
//  * Source: Aho, Sethi, and Ullman, "Compilers", Addison-Wesley, 1986 (page 436).
//  */
// int hashpjw( char *s )
// {
// 	char *p; 
// 	unsigned h = 0, g; 
	
// 	for ( p = s; *p != EOS; p++ ) 
// 	{ 
// 		h = (h << 4) + (*p); 
// 		if ( g = h & 0xf0000000 ) 
// 		{ 
// 			h = h ^ ( g >> 24 ); 
// 			h = h ^ g; 
// 		} 
// 	} 
// 	return h % TABLE_SIZE; 
// }



// void putInTable(Hashtable &hashtable, char *id, int value) {
	
// }

// int getInTable(Hashtable &hashtable, char *id) {

// }

// void clearTable(Hashtable &hashtable){

// }

// int sizeTable(Hashtable &hashtable) {

// }

// void printTable(Hashtable &hashtable){

// }