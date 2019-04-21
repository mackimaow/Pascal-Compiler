#include <stdlib.h>
#include <stdio.h>
#include "linkedlist.h"
#include "utils.h"
#include "object.h"

LinkedList * linkedlist;

void printElem(ObjectType * type, void* element) {
	printf("%c", *((char*)element));
}

void deconstElem(ObjectType * type, void* element) {
	// free(element);
}

static ObjectType * CHAR;

int main(void) {
	CHAR = objectTypeInit(printElem, 0, deconstElem);

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


	linkedListDestroy(linkedList);
	return 0;
}
