#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdbool.h>
#include "utils.h"
#include "object.h"
#include "iterator.h"

typedef struct LinkedList LinkedList;
typedef struct LinkedNode LinkedNode;

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

Iterator * linkedListIteratorInit(LinkedList * linkedList);
Iterator * linkedListIteratorInitBack(LinkedList * linkedList);


#endif // LINKEDLIST_H