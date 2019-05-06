#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdbool.h>
#include "utils.h"
#include "object.h"

typedef struct LinkedList LinkedList;
typedef struct LinkedNode LinkedNode;
typedef struct Iterator Iterator;

LinkedList* linkedListInit ( ObjectType * elementType ) ;
LinkedList* linkedListInitWithPrintProperties ( ObjectType * elementType, ListPrintProperties * printProperties);
void linkedListDestroy(  LinkedList * linkedList  );

void linkedListChangePrintProperties( LinkedList * linkedList, ListPrintProperties * printProperties );
void  linkedListPush (  LinkedList * linkedList,  void * element );
void*  linkedListPop  (  LinkedList * linkedList );
void*  linkedListPeak  (  LinkedList * linkedList );


void linkedListEnqueue( LinkedList * linkedList,  void * element );
void* linkedListDequeue( LinkedList * linkedList );

void linkedListPushBack( LinkedList * linkedList,  void * element );
void* linkedListPopBack( LinkedList * linkedList );
void* linkedListPeakBack( LinkedList * linkedList );


void  linkedListAdd (  LinkedList * linkedList,  void * element, int index );
bool  linkedListRemove (  LinkedList * linkedList,  int i );
bool  linkedListRemoveElement (  LinkedList * linkedList,  void * element );
void* linkedListGet (  LinkedList * linkedList,  int i );

void linkedListClear (  LinkedList * linkedList  );
int  linkedListGetSize (  LinkedList * linkedList  );
void linkedListPrint (  LinkedList * linkedlist  );
char* linkedListToString (  LinkedList * linkedlist  );

bool  linkedListForEach (  LinkedList * linkedList, ForEach forEach, int numArgs, ...);


// Iterator functions

Iterator * iteratorInit(LinkedList * linkedList);
void iteratorDestroy(Iterator * iterator);
Iterator * iteratorInitBack(LinkedList * linkedList);
bool iteratorHasNext(Iterator * iterator);
bool iteratorHasPrevious(Iterator * iterator);
void * iteratorGetNext(Iterator * iterator); 
void * iteratorGetPrevious(Iterator * iterator); 


#endif // LINKEDLIST_H