#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "linkedlist.h"
#include "utils.h"


struct LinkedNode {
	void * element;
	struct LinkedNode * next;
};


struct LinkedList {
	int size;
	ObjectType * elementType;
	ListPrintProperties * printProperties;
	LinkedNode * first;
};


// Util functions

static void nodeDelete( LinkedNode * node , ObjectType * elementType ) {
	objectTypeDestroyValue(elementType, node->element);
	free(node);
}


static void nodeDeleteRec( LinkedNode * node, ObjectType * elementType ) {
	if( !node )
		return;
	nodeDeleteRec(node->next, elementType);
	nodeDelete(node, elementType);
}

static LinkedNode * nodeCreate(void* element, LinkedNode * nextNode) {
	LinkedNode * tempNode 	= (LinkedNode*)malloc(sizeof(LinkedNode));
	tempNode->element 		= element;
	tempNode->next 			= nextNode;
	return tempNode;
}


static bool addForEach(ObjectType * elementType, int index, void* element, void** parameters, ForEachOptions* options) {
	if(*((int*)parameters[0]) == index) {
		options->insert = parameters[1];
		return true;
	}
	return false;
}


static bool removeForEach(ObjectType * elementType, int index, void* element, void** parameters, ForEachOptions* options) {
	if(*((int*)parameters[0]) == index) {
		options->remove = true;
		return true;
	}
	return false;
}


static bool getForEach(ObjectType * elementType, int index, void* element, void** parameters, ForEachOptions* options) {
	if(*((int*)parameters[0]) == index) {
		Single * single = (Single*)(parameters[1]); 
		single->element = element;
		return true;
	}
	return false;
}

static bool printForEach(ObjectType * elementType, int index, void* element, void** parameters, ForEachOptions* options) {
	bool * printedFirstElement = (bool *)parameters[0];
	if(*printedFirstElement) {
		char * separatorString = (char*)parameters[1];
		printf("%s", separatorString);
	} else {
		*printedFirstElement = true;
	}
	objectTypePrintValue(elementType, element);
	return false;
}


// Definitions
LinkedList* linkedListInit ( ObjectType * elementType ) {
	linkedListInitWithPrintProperties ( elementType, &defaultPrintProperties);	
}

LinkedList* linkedListInitWithPrintProperties ( ObjectType * elementType, ListPrintProperties * printProperties) {
	LinkedList * linkedList = (LinkedList*)malloc(sizeof(LinkedList));
	linkedList->elementType = elementType;
	linkedList->size = 0;
	linkedList->printProperties = printProperties;
	linkedList->first = 0;
	return linkedList;
}


void linkedListDestroy(  LinkedList * linkedList  ) {
	nodeDeleteRec ( linkedList->first , linkedList->elementType );
	free(linkedList);
}

void  linkedListPush (  LinkedList * linkedList,  void * element ) {
	linkedList->first = nodeCreate(element, linkedList->first);
	linkedList->size++;
}


void*  linkedListPop  (  LinkedList * linkedList) {
	if(linkedList->first) {
		LinkedNode * toDelete = linkedList->first; 
		void* elem = toDelete->element;
		linkedList->first = toDelete->next;
		nodeDelete(toDelete, linkedList->elementType);
		linkedList->size--;
		return elem;
	}
	return 0;
}

void*  linkedListPeak  (  LinkedList * linkedList ) {
	if(linkedList->first)
		return linkedList->first->element;
	return 0;	
}

void  linkedListAdd (  LinkedList * linkedList, void * element, int index ) {
	linkedListForEach(linkedList, addForEach, 2, &index, element);
}

void  linkedListRemove (  LinkedList * linkedList,  int i ) {
	linkedListForEach(linkedList, removeForEach, 1, &i);
}

void* linkedListGet (  LinkedList * linkedList,  int i ) {
	Single single;
	linkedListForEach (linkedList, getForEach, 2, &i, &single);
	return single.element;
}

void linkedListClear (  LinkedList * linkedList  ) {
	linkedList->size = 0;
	nodeDeleteRec(linkedList->first, linkedList->elementType);
	linkedList->first = 0;
}

int  linkedListGetSize (  LinkedList * linkedList  ) {
	return linkedList->size;
}

void linkedListPrint (  LinkedList * linkedList ) {
	printf("%s", linkedList->printProperties->startString);
	bool printedFirstElement = false;
	linkedListForEach( linkedList, printForEach, 2, &printedFirstElement, linkedList->printProperties->separatorString);
	printf("%s", linkedList->printProperties->endString);
}

bool linkedListForEach (  LinkedList * linkedList, ForEach forEach, int numArgs, ... ) {
	ForEachOptions * options = (ForEachOptions*) malloc(sizeof(ForEachOptions));
	resetOptions(options);
	void ** parameterArray = malloc(sizeof(void*) * numArgs);

	va_list    argList;
	va_start( argList, numArgs );
	for( int i = 0; i < numArgs; i++ )
    	*(parameterArray + i) = va_arg( argList, void* );
  	va_end( argList );

	int index = 0;
	bool stopShort = false;
	LinkedNode * curNode = linkedList->first;
	LinkedNode * prevNode = 0;

	while (curNode) {
		
		stopShort = forEach(linkedList->elementType, index, curNode->element, parameterArray, options);

		LinkedNode * nextNode = curNode->next;
		int indexInr = (options->remove? -1 : 0) + (options->insert? 1 : 0);

		if(indexInr == -1) {
			nodeDelete(curNode, linkedList->elementType);
			if(index == 0)
				linkedList->first = nextNode;					
			else
				prevNode->next = nextNode;
			linkedList->size--;


		} else if (indexInr == 1) {
			LinkedNode * tempNode = nodeCreate(options->insert, curNode);
			if(index == 0)
				linkedList->first = tempNode;
			else
				prevNode->next = tempNode;
			linkedList->size++;

		} else if (options->insert) {
			curNode->element = options->insert;
		}

		if(stopShort)
			break;

		index = index + 1 + indexInr;
		prevNode=curNode;
		curNode=nextNode;
		resetOptions(options);
	}

	free(parameterArray);
	free(options);
	return stopShort;
}