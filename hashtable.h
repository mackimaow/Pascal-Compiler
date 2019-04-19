#ifndef HASHTABLE_H
#define HASHTABLE_H


#include "linkedlist.h"


#define TABLE_SIZE	211
#define EOS		'\0'


#define struct Hashtable {
	int size = 0;
	LinkedList buckets[TABLE_SIZE];
};


void putInTable(Hashtable &hashtable, char *id, int value);
int getInTable(Hashtable &hashtable, char *id);
void clearTable(Hashtable &hashtable);
int sizeTable(Hashtable &hashtable);


#endif // HASHTABLE_H
