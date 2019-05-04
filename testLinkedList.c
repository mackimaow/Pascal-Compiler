#include <stdlib.h>
#include <stdio.h>
#include "linkedlist.h"
#include "utils.h"
#include "object.h"

LinkedList * linkedlist;

char * toCharString(ObjectType * type, void* element) {
	char * stringValue = (char *) malloc(sizeof(char)*2);
	stringValue[0] = *(char *)element;
	stringValue[1] = '\0';
	return stringValue;
}

void deconstElem(ObjectType * type, void* element) {
	// free(element);
}

static ObjectType * CHAR;

int main(void) {
	CHAR = objectTypeInit(toCharString, 0, deconstElem);

	LinkedList * linkedList = linkedListInit(CHAR);
	char * c = "Yo yo yo";
	for(int i = 0; i < 8; i++)
		linkedListPush( linkedList, c + i);
	linkedListPrint(linkedList);
	printf("\n");
	int size = linkedListGetSize(linkedList);
	printf("Size: %d\n", size);

	char temp1 = 'h';
	linkedListAdd(linkedList, &temp1, 2);
	size = linkedListGetSize(linkedList);
	linkedListPrint(linkedList);
	printf("\n");
	printf("Size: %d\n", size);

	char temp2 = 'l';
	linkedListAdd(linkedList, &temp2, 0);
	size = linkedListGetSize(linkedList);
	linkedListPrint(linkedList);
	printf("\n");
	printf("Size: %d\n", size);

	char temp3 = *(char*)linkedListPop(linkedList);
	size = linkedListGetSize(linkedList);
	linkedListPrint(linkedList);
	printf("\n");
	printf("Size: %d\n", size);

	linkedListAdd(linkedList, &temp3, 7);
	size = linkedListGetSize(linkedList);
	linkedListPrint(linkedList);
	printf("\n");
	printf("Size: %d\n", size);
	
	linkedListRemove(linkedList, 3);
	size = linkedListGetSize(linkedList);
	linkedListPrint(linkedList);
	printf("\n");
	printf("Size: %d\n", size);

	linkedListClear(linkedList);
	size = linkedListGetSize(linkedList);
	linkedListPrint(linkedList);
	printf("\n");
	printf("Size: %d\n", size);

	for(int i = 0; i < 8; i++)
		linkedListPushBack( linkedList, c + i);
	linkedListPrint(linkedList);
	printf("\n");
	size = linkedListGetSize(linkedList);
	printf("Size: %d\n", size);


	for(int i = 0; i < 8; i++) {
		char * cd = (char*)linkedListDequeue( linkedList );
		linkedListPrint(linkedList);
		printf("\n");
		printf("Size: %d Element Dequeued: %c\n", size, *cd);				
	}


	linkedListDestroy(linkedList);
	return 0;
}
