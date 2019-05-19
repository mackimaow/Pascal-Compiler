#include <stdlib.h>
#include <stdarg.h>
#include "iterator.h"


struct Iterator {
	IteratorHasNextFunction * iteratorHasNextFunction;
	IteratorHasPreviousFunction * iteratorHasPreviousFunction;
	IteratorGetNextFunction * iteratorGetNextFunction;
	IteratorGetPreviousFunction * iteratorGetPreviousFunction;
	IteratorDestructor * iteratorDestructor;
	int numIteratorFields;
	void ** iteratorFields;
};


Iterator * iteratorInit(IteratorHasNextFunction * iteratorHasNextFunction, IteratorHasPreviousFunction * iteratorHasPreviousFunction, IteratorGetNextFunction * iteratorGetNextFunction,
 						IteratorGetPreviousFunction * iteratorGetPreviousFunction, IteratorDestructor * iteratorDestructor, int numIteratorFields, ...) {
	Iterator * iterator = malloc(sizeof(Iterator));
	iterator->iteratorHasNextFunction = iteratorHasNextFunction;
	iterator->iteratorHasPreviousFunction = iteratorHasPreviousFunction;
	iterator->iteratorGetNextFunction = iteratorGetNextFunction;
	iterator->iteratorGetPreviousFunction = iteratorGetPreviousFunction;
	iterator->iteratorDestructor = iteratorDestructor;
	iterator->numIteratorFields = numIteratorFields;
	iterator->iteratorFields = malloc(sizeof(void *) * numIteratorFields);
	va_list   argList;
	va_start( argList, numIteratorFields);
	for( int i = 0; i < numIteratorFields; i++ )
    	iterator->iteratorFields[i] = va_arg( argList, void * );
    va_end( argList );
    return iterator;
}

void iteratorDestroy(Iterator * iterator) {
	if (iterator->iteratorDestructor)
		iterator->iteratorDestructor( iterator->numIteratorFields, iterator->iteratorFields);
	free(iterator->iteratorFields);
	free(iterator);
}

bool iteratorHasNext(Iterator * iterator) {
	if (iterator->iteratorHasNextFunction)
		return iterator->iteratorHasNextFunction(iterator->numIteratorFields, iterator->iteratorFields);
	return false;
}

bool iteratorHasPrevious(Iterator * iterator) {
	if (iterator->iteratorHasPreviousFunction)
		return iterator->iteratorHasPreviousFunction(iterator->numIteratorFields, iterator->iteratorFields);
	return false;	
}

void * iteratorGetNext(Iterator * iterator) {
	if (iterator->iteratorGetNextFunction)
		return iterator->iteratorGetNextFunction(iterator->numIteratorFields, iterator->iteratorFields);
	return 0;
}

void * iteratorGetPrevious(Iterator * iterator){
	if (iterator->iteratorGetPreviousFunction)
		return iterator->iteratorGetPreviousFunction(iterator->numIteratorFields, iterator->iteratorFields);
	return 0;
}