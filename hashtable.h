#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "linkedlist.h"

typedef struct HashTable HashTable;
typedef int HashFunction (void* key);

HashTable* hashTableInit( int capacity, HashFunction * hashFunction, ObjectType * keyType, ObjectType * elementType );
HashTable* hashTableInitWithPrintProperties( int capacity, HashFunction * hashFunction, ObjectType * keyType, ObjectType * elementType, MapPrintProperties * printProperties);
void hashTableDestroy(HashTable * hashtable);
void hashTablePut(HashTable * hashtable, void* key, void* element);
void* hashTableGet(HashTable * hashtable, void* key);
void hashTableRemove(HashTable * hashtable, void* key);
void hashTableClear(HashTable * hashtable);
int  hashTableSize(HashTable * hashtable);
int  hashTableCapacity(HashTable * hashtable);
void printTable(HashTable * hashtable);

#endif // HASHTABLE_H
