#include <stdlib.h>
#include "linkedlist.h"
#include <string.h>

// static struct LinkedPairElement {
// 	char * id;
// 	int value;
// 	struct LinkedElement *next = nullptr;
// }

// #define struct LinkedList {
// 	int size = 0;
// 	struct LinkedPairElement *first = nullptr;
// };

static LinkedList* nodeCreate( char * attribute,  char * tokenType ) {
	struct LinkedElement* node = (struct LinkedElement*) malloc(sizeof(struct LinkedElement));
	node->attribute = attribute;
	node->tokenType = tokenType;
	node->next = nullptr;
	return node;
}


static void nodeDelete( struct LinkedElement * element ) {
	if( element == nullptr )
		return;

	nodeDelete(element->next);
	
	free(element->attribute);
	free(element);
}


static int nodeAdd ( struct LinkedElement * element, int currentIndex, char * attribute,  char * tokenType ) {
	if(strcmp(attribute, element->attribute) == 0) {
		element->attribute = attribute;
		return currentIndex;

	} else if (element->next) {
		return nodeAdd(element->next, currentIndex + 1, attribute, tokenType);
	
	} else{
		element->next = nodeCreate(char * attribute, int tokenType);
		return currentIndex + 1;
	
	}
}



LinkedList* linkedListInit () {
	LinkedList* linkedList = (LinkedList*) malloc(sizeof(LinkedList));
	return linkedList;
}



void linkedListDestroy(  LinkedList* linkedList  ) {
	nodeDelete ( linkedList->first );
	free(linkedList);
}


int linkedListAdd (  LinkedList* linkedList,  char * attribute,  char * tokenType  ) {

	if(linkedList->first) {
		int index = nodeAdd ( linkedList->first, 0, attribute, tokenType );
		if( index = linkedList->size )
			linkedList->size = index + 1;
		return index;

	} else{
		linkedList->first = nodeCreate(attribute, tokenType);
		linkedList->size = 1;
		return 0;
	}
}

int linkedListRemove (  LinkedList* linkedList,  char * attribute  ) {

}

void linkedListClear (  LinkedList* linkedList  ) {
	free(linkedList->first);
	linkedlist->size = 0;
}

int  linkedListGet (  LinkedList* linkedList,  char * attribute  ) {
	if(linkedlist->size = 0)
		return -1;
}

int  linkedListGetSize (  LinkedList* linkedList  ) {
	return linkedList->size;
}

void linkedListPrint (  LinkedList* linkedlist  ) {

}