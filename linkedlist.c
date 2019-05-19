#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "linkedlist.h"
#include "utils.h"
#include "object.h"


struct LinkedNode {
	void * element;
	struct LinkedNode * next;
	struct LinkedNode * previous;
};


struct LinkedList {
	int size;
	ObjectType * elementType;
	ListPrintProperties * printProperties;
	LinkedNode * first;
	LinkedNode * last;
};

struct Iterator {
	LinkedNode * next;
	LinkedNode * previous;
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

static void nodeCreate(LinkedList * linkedList, void* element, LinkedNode * currentNode) {
	LinkedNode * prevNode 	= currentNode? currentNode->previous : 0;
	LinkedNode * nextNode 	= currentNode;

	LinkedNode * tempNode 	= (LinkedNode*)malloc(sizeof(LinkedNode));
	tempNode->element 		= element;
	tempNode->previous		= prevNode;
	tempNode->next 			= nextNode;
	if(prevNode)
		prevNode->next = tempNode;
	else
		linkedList->first = tempNode;
	if(nextNode)
		nextNode->previous = tempNode;
	else
		linkedList->last = tempNode;
	linkedList->size++;
}

static void nodeCreateLast(LinkedList * linkedList, void* element) {
	LinkedNode * previousLastNode = linkedList->last;

	LinkedNode * tempNode 	= (LinkedNode*)malloc(sizeof(LinkedNode));
	tempNode->element 		= element;
	tempNode->previous		= previousLastNode;
	tempNode->next 			= 0;

	if(previousLastNode)
		previousLastNode->next = tempNode;
	else
		linkedList->first = tempNode;
	linkedList->last = tempNode;
	linkedList->size++;
}

static void nodeRemove(LinkedList * linkedList, LinkedNode * currentNode) {
	LinkedNode * prevNode 	= currentNode? currentNode->previous : 0;
	LinkedNode * nextNode 	= currentNode? currentNode->next : 0;
	nodeDelete(currentNode, linkedList->elementType);
	if(prevNode)
		prevNode->next = nextNode;
	else
		linkedList->first = nextNode;
	if(nextNode)
		nextNode->previous = prevNode;
	else
		linkedList->last = prevNode;
	linkedList->size--;
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

static bool removeElementForEach(ObjectType * elementType, int index, void* element, void** parameters, ForEachOptions* options){
	if(element == parameters[0]) {
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

static bool toStringForEach(ObjectType * elementType, int index, void* element, void** parameters, ForEachOptions* options) {
	LinkedList * linkedList = (LinkedList *) parameters[0];
	LinkedList * stringLengthsList = (LinkedList *) parameters[1];
	Single single = *(Single *)parameters[2];
	char * elementString = objectTypeToString(elementType, element);
	int size = 0;
	char c = *elementString;
	int * stringSize = malloc(sizeof(int));
	*(stringSize) = getStringSize(elementString);
	int * totalSize = (int *)(single.element);
	*(totalSize) += getStringSize(elementString);
	linkedListPush(linkedList, elementString);
	linkedListPush(stringLengthsList, stringSize); 
	return false;
}


// Definitions
LinkedList* linkedListInit ( ObjectType * elementType ) {
	linkedListInitWithPrintProperties ( elementType, &defaultPrintProperties);	
}

LinkedList* linkedListInitWithPrintProperties ( ObjectType * elementType, ListPrintProperties * printProperties) {
	LinkedList * linkedList = (LinkedList*) malloc(sizeof(LinkedList));
	linkedList->elementType = elementType;
	linkedList->size = 0;
	linkedList->printProperties = printProperties;
	linkedList->first = 0;
	linkedList->last = 0;
	return linkedList;
}

void linkedListDestroy(  LinkedList * linkedList  ) {
	nodeDeleteRec ( linkedList->first , linkedList->elementType );
	free(linkedList);
}

void linkedListChangePrintProperties( LinkedList * linkedList, ListPrintProperties * printProperties ) {
	linkedList->printProperties = printProperties;
}


void  linkedListPush (  LinkedList * linkedList,  void * element ) {
	nodeCreate(linkedList, element, linkedList->first);
}


void*  linkedListPop  (  LinkedList * linkedList) {
	if(linkedList->first) {
		LinkedNode * toDelete = linkedList->first; 
		void* elem = toDelete->element;
		nodeRemove(linkedList, toDelete);
		return elem;
	}
	return 0;
}

void*  linkedListPeak  (  LinkedList * linkedList ) {
	if(linkedList->first)
		return linkedList->first->element;
	return 0;	
}


void linkedListEnqueue( LinkedList * linkedList,  void * element ) {
	linkedListPush (  linkedList,  element );
}

void* linkedListDequeue( LinkedList * linkedList ) {
	return linkedListPopBack( linkedList );
}


void linkedListPushBack( LinkedList * linkedList,  void * element ) {
	nodeCreateLast(linkedList, element);
}

void* linkedListPopBack( LinkedList * linkedList ) {
	if(linkedList->last) {
		LinkedNode * toDelete = linkedList->last; 
		void* elem = toDelete->element;
		nodeRemove(linkedList, toDelete);
		return elem;
	}
	return 0;	
}

void* linkedListPeakBack( LinkedList * linkedList ) {
	if(linkedList->last)
		return linkedList->last->element;
	return 0;
}



void  linkedListAdd (  LinkedList * linkedList, void * element, int index ) {
	linkedListForEach(linkedList, addForEach, 2, &index, element);
}

bool  linkedListRemove (  LinkedList * linkedList,  int i ) {
	return linkedListForEach(linkedList, removeForEach, 1, &i);
}

bool  linkedListRemoveElement (  LinkedList * linkedList,  void * element ) {
	return linkedListForEach(linkedList, removeElementForEach, 1, element);
}

void* linkedListGet (  LinkedList * linkedList,  int i ) {
	Single single;
	single.element = 0;
	linkedListForEach (linkedList, getForEach, 2, &i, &single);
	return single.element;
}

void linkedListClear (  LinkedList * linkedList  ) {
	linkedList->size = 0;
	nodeDeleteRec(linkedList->first, linkedList->elementType);
	linkedList->first = 0;
	linkedList->last = 0;
}

int  linkedListGetSize (  LinkedList * linkedList  ) {
	return linkedList->size;
}

void linkedListPrint (  LinkedList * linkedList ) {
	if(!objectTypeHasToStringFunction(linkedList->elementType))
		return;
	char * stringList = linkedListToString (  linkedList  );
	printf("%s", stringList);
	free(stringList);
}

char* linkedListToString (  LinkedList * linkedList  ) {
	if(!objectTypeHasToStringFunction(linkedList->elementType))
		return 0;
	ListPrintProperties * printProperties = linkedList->printProperties;

	LinkedList * stringList        = linkedListInit(&STRING_OBJECT);
	LinkedList * stringLengthsList = linkedListInit(&INT_OBJECT);
	int sumStringLengths = 0;
	Single single;
	single.element = &sumStringLengths;

	linkedListForEach( linkedList, toStringForEach, 3, stringList, stringLengthsList, &single);
	
	int numberOfElements = linkedListGetSize( stringList );
	int startStringLength = getStringSize(printProperties->startString);
	int separatorStringLength = getStringSize(printProperties->separatorString);
	int endStringLength = getStringSize(printProperties->endString);

	int numberOfSeparators = numberOfElements-1;
	if(numberOfSeparators < 0)
		numberOfSeparators = 0;

	int totalStringLength = startStringLength + sumStringLengths + separatorStringLength * numberOfSeparators + endStringLength;

	char * temp =  malloc(sizeof(char) * (totalStringLength+1));

	*(temp+totalStringLength) = '\0';
	
	int indexFromBack = totalStringLength-endStringLength;
	stringInsert(temp, printProperties->endString, indexFromBack);

	for(int i = numberOfElements - 1; i>=1; i--) {
		char * stringToInsert = (char *) linkedListPeak(stringList);
		int stringLength = *(int *) linkedListPeak(stringLengthsList);
		indexFromBack-=stringLength;
		stringInsert(temp, stringToInsert, indexFromBack);
		indexFromBack-=separatorStringLength;
		stringInsert(temp, printProperties->separatorString, indexFromBack);
		linkedListPop(stringList);
		linkedListPop(stringLengthsList);
	}
	if(linkedListGetSize(stringList) != 0) {
		char * stringToInsert = (char *) linkedListPeak(stringList);
		int stringLength = *(int *) linkedListPeak(stringLengthsList);
		indexFromBack-=stringLength;
		stringInsert(temp, stringToInsert, indexFromBack);
	}
	stringInsert(temp, printProperties->startString, 0);

	linkedListDestroy(stringList);
	linkedListDestroy(stringLengthsList);
	return temp;
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

		if(indexInr == -1)
			nodeRemove(linkedList, curNode);
		else if (indexInr == 1)
			nodeCreate(linkedList, options->insert, curNode);
		else if (options->insert)
			curNode->element = options->insert;

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


static bool linkedListIteratorHasNext(int numIteratorFields, void ** iteratorFields) {
	return iteratorFields[1]? true : false;
}

static bool linkedListIteratorHasPrevious(int numIteratorFields, void ** iteratorFields) {
	return iteratorFields[0]? true : false;
}

static void * linkedListIteratorGetNext(int numIteratorFields, void ** iteratorFields) {
	LinkedNode * next = (LinkedNode *) iteratorFields[1];
	if(next) {
		iteratorFields[0] = next;
		iteratorFields[1] = next->next;
		return next->element;
	}
	return 0;
}

static void * linkedListIteratorGetPrevious(int numIteratorFields, void ** iteratorFields) {
	LinkedNode * previous = (LinkedNode *) iteratorFields[0];
	if(previous) {
		iteratorFields[1] = previous;
		iteratorFields[0] = previous->previous;
		return previous->element;
	}
	return 0;
}

Iterator * linkedListIteratorInit(LinkedList * linkedList) {
	Iterator * iterator = iteratorInit(linkedListIteratorHasNext, linkedListIteratorHasPrevious, linkedListIteratorGetNext,
 										linkedListIteratorGetPrevious, 0, 2, 0, linkedList->first);
	return iterator;
}

Iterator * linkedListIteratorInitBack(LinkedList * linkedList) {
	Iterator * iterator = iteratorInit(linkedListIteratorHasNext, linkedListIteratorHasPrevious, linkedListIteratorGetNext,
 										linkedListIteratorGetPrevious, 0, 2, linkedList->last, 0);
	return iterator;
}
