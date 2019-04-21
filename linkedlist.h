#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdbool.h>
#include "utils.h"
#include "object.h"

typedef struct LinkedList LinkedList;
typedef struct LinkedNode LinkedNode ;

LinkedList* linkedListInit ( ObjectType * elementType ) ;
LinkedList* linkedListInitWithPrintProperties ( ObjectType * elementType, ListPrintProperties * printProperties);
void linkedListDestroy(  LinkedList * linkedList  );

void  linkedListPush (  LinkedList * linkedList,  void * element );
void*  linkedListPop  (  LinkedList * linkedList );
void*  linkedListPeak  (  LinkedList * linkedList );
void  linkedListAdd (  LinkedList * linkedList,  void * element, int index );
void  linkedListRemove (  LinkedList * linkedList,  int i );
void* linkedListGet (  LinkedList * linkedList,  int i );

void linkedListClear (  LinkedList * linkedList  );
int  linkedListGetSize (  LinkedList * linkedList  );
void linkedListPrint (  LinkedList * linkedlist  );

bool  linkedListForEach (  LinkedList * linkedList, ForEach forEach, int numArgs, ...);

#endif // LINKEDLIST_H