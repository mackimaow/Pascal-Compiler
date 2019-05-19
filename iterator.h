#ifndef ITERATOR_H
#define ITERATOR_H

#include <stdbool.h>

typedef struct Iterator Iterator; 

typedef bool  IteratorHasNextFunction(int numIteratorFields, void ** iteratorFields);
typedef bool  IteratorHasPreviousFunction(int numIteratorFields, void ** iteratorFields);
typedef void* IteratorGetNextFunction(int numIteratorFields, void ** iteratorFields);
typedef void* IteratorGetPreviousFunction(int numIteratorFields, void ** iteratorFields);
typedef void  IteratorDestructor(int numIteratorFields, void ** iteratorFields);


Iterator * iteratorInit(IteratorHasNextFunction * iteratorHasNextFunction, IteratorHasPreviousFunction * iteratorHasPreviousFunction, IteratorGetNextFunction * iteratorGetNextFunction,
 						IteratorGetPreviousFunction * iteratorGetPreviousFunction, IteratorDestructor * iteratorDestructor, int numIteratorFields, ...);
void iteratorDestroy(Iterator * iterator);
bool iteratorHasNext(Iterator * iterator);
bool iteratorHasPrevious(Iterator * iterator);
void * iteratorGetNext(Iterator * iterator);
void * iteratorGetPrevious(Iterator * iterator);


#endif // ITERATOR_H