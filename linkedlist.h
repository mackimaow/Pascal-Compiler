#ifndef LINKEDLIST_H
#define LINKEDLIST_H



struct LinkedPairElement {
	char * attribute;
	char * tokenType;
	struct LinkedPairElement * next = nullptr;
} LinkedPairElement ; 

typedef struct LinkedList {
	int size = 0;
	struct LinkedPairElement * first = nullptr;
};


LinkedList* linkedListInit ( ) ;
void linkedListDestroy(  LinkedList* linkedList  );
int  linkedListAdd (  LinkedList & linkedList,  char * attribute,  char * tokenType  );
void linkedListRemove (  LinkedList & linkedList,  char * attribute  );
void linkedListClear (  LinkedList & linkedList  );
int  linkedListGet (  LinkedList & linkedList,  char * attribute  );
int  linkedListGetSize (  LinkedList & linkedList  );
void linkedListPrint (  LinkedList & linkedlist  );

#endif // LINKEDLIST_H