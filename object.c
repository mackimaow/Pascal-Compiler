#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "object.h"

struct ObjectType {
	PrintFunction * printFunction;
	CompareFunction * compareFunction;
	Destructor * destructor;
	int nestedTypesSize;
	ObjectType ** nestedTypes;
	int numProperties;
	void ** properties;
};

ObjectType * objectTypeInit(PrintFunction * printFunction, CompareFunction * compareFunction, Destructor * destructor) {
	return objectTypePropertiesNestedInit(printFunction, compareFunction, destructor, 0, 0);
}

ObjectType * objectTypePropertiesNestedInit(PrintFunction * printFunction, CompareFunction * compareFunction, Destructor * destructor, int nestedTypesSize, int numProperties, ...) {
	ObjectType * objType = malloc(sizeof(ObjectType));
	objType->printFunction = printFunction;
	objType->compareFunction = compareFunction;
	objType->destructor = destructor;
	objType->nestedTypesSize = nestedTypesSize;
	objType->numProperties = numProperties;

	va_list   argList;
	va_start( argList, numProperties);
	if(nestedTypesSize > 0) {
		objType->nestedTypes = malloc(sizeof(ObjectType*) * nestedTypesSize);
		for( int i = 0; i < nestedTypesSize; i++ )
    		*(objType->nestedTypes + i) = va_arg( argList, ObjectType * );
  		
  	} else{
  		objType->nestedTypes = 0;
  	}
  	if(numProperties > 0) {
		objType->properties = malloc(sizeof(void *) * numProperties);
		for( int i = 0; i < nestedTypesSize; i++ )
    		*(objType->properties + i) = va_arg( argList, void * );
  	} else{
  		objType->properties = 0;
  	}
  	va_end( argList );
  	return objType;
}

void objectTypeDestroyTypeOnly(ObjectType * objectType) {
	free(objectType->nestedTypes);
	free(objectType);
}

void objectTypeDestroyValue(ObjectType * objectType, void * value) {
	objectType->destructor(objectType, value);
}

void objectTypePrintValue(ObjectType * objectType, void * value) {
	objectType->printFunction(objectType, value);
}

ObjectType * objectTypeGetNestedType(ObjectType * objectType, int index) {
	int size = objectType->nestedTypesSize;
	if(index >= 0 && index < size)
		return objectType->nestedTypes[index];
	return 0;
}

void * objectTypeGetProperty(ObjectType * objectType, int index) {
	int size = objectType->numProperties;
	if(index >= 0 && index < size)
		return objectType->properties[index];
	return 0;
}


int objectTypeCompare(ObjectType * objectType, void * value1, void * value2) {
	return objectType->compareFunction(objectType, value1, value2);
}

int objectTypeGetNestedTypesSize(ObjectType * objectType) {
	return objectType->nestedTypesSize;
}

int objectTypeGetPropertiesSize(ObjectType * objectType) {
	return objectType->numProperties;
}