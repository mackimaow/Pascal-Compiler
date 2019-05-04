#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "hashtable.h"
#include "object.h"




void destroyString (ObjectType * objectType, void * value) {
	//
}

int compareString (ObjectType * objectType, void * value1, void * value2) {
	return strcmp((char*)value1, (char*)value2);
}

char * stringToString(ObjectType * objectType, void * value) {
	char* str = (char*)value;
	int size = getStringSize(str);
	char * temp = malloc(sizeof(char) * (size + 1));
	stringInsert(temp, str, 0);
	temp[size] = '\0';
	return temp;
}

static ObjectType * STRING_TYPE;

#define TABLE_SIZE	211
#define EOS		'\0'

/* ----------------------------------------------------------------------------- 
 * hashpjw
 * Peter J. Weinberger's hash function 
 * Source: Aho, Sethi, and Ullman, "Compilers", Addison-Wesley, 1986 (page 436).
 */
int hashpjw( void* value )
{
	char *s = (char*)value; 
	char *p; 
	unsigned h = 0, g; 
	
	for ( p = s; *p != EOS; p++ ) 
	{ 
		h = (h << 4) + (*p); 
		if ( g = h & 0xf0000000 ) 
		{ 
			h = h ^ ( g >> 24 ); 
			h = h ^ g; 
		} 
	} 
	return h % TABLE_SIZE; 
}


int main() {
	STRING_TYPE = objectTypeInit(stringToString, compareString, destroyString);
	HashTable * table = hashTableInit(TABLE_SIZE, hashpjw, STRING_TYPE, STRING_TYPE);

	hashTablePrint(table);
	printf("\n");
	printf("Size: %d\n", hashTableSize(table));

	hashTablePut(table, "One", "Another");
	hashTablePrint(table);
	printf("\n");
	printf("Size: %d\n", hashTableSize(table));

	hashTablePut(table, "two", "Joke");
	hashTablePrint(table);
	printf("\n");
	printf("Size: %d\n", hashTableSize(table));

	hashTablePut(table, "Three", "John");
	hashTablePrint(table);
	printf("\n");
	printf("Size: %d\n", hashTableSize(table));
	
	hashTablePut(table, "two", "John");
	hashTablePrint(table);
	printf("\n");
	printf("Size: %d\n", hashTableSize(table));
	
	hashTableRemove(table, "Three");
	hashTablePrint(table);
	printf("\n");
	printf("Size: %d\n", hashTableSize(table));

	hashTableRemove(table, "Three");
	hashTablePrint(table);
	printf("\n");
	printf("Size: %d\n", hashTableSize(table));

	char* element = hashTableGet(table, "One");
	printf("One is: %s\n", element);

	hashTableClear(table);
	hashTablePrint(table);
	printf("\n");
	printf("Size: %d\n", hashTableSize(table));

	hashTableDestroy(table);
}