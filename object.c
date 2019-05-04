#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "object.h"
#include "utils.h"

struct ObjectType {
	ToStringFunction * toStringFunction;
	CompareFunction * compareFunction;
	Destructor * destructor;
	int nestedTypesSize;
	ObjectType ** nestedTypes;
	int numProperties;
	void ** properties;
};


static char* CHAR_TO_STRING(ObjectType * objectType, void * value);
static int CHAR_COMPARE(ObjectType * objectType, void * value1, void * value2);
static void CHAR_DESTRUCTOR(ObjectType * objectType, void * value);

static char* STRING_TO_STRING(ObjectType * objectType, void * value);
static int STRING_COMPARE(ObjectType * objectType, void * value1, void * value2);
static void STRING_DESTRUCTOR(ObjectType * objectType, void * value);

static char* INT_TO_STRING(ObjectType * objectType, void * value);
static int INT_COMPARE(ObjectType * objectType, void * value1, void * value2);
static void INT_DESTRUCTOR(ObjectType * objectType, void * value);

ObjectType CHAR_OBJECT 	= {CHAR_TO_STRING, CHAR_COMPARE, CHAR_DESTRUCTOR, 0, 0, 0, 0};
ObjectType STRING_OBJECT 	= {STRING_TO_STRING, STRING_COMPARE, STRING_DESTRUCTOR, 0, 0, 0, 0};
ObjectType INT_OBJECT 	= {INT_TO_STRING, INT_COMPARE, INT_DESTRUCTOR, 0, 0, 0, 0};

static ObjectType * NULL_TYPE; 




ObjectType * objectTypeInit(ToStringFunction * toStringFunction, CompareFunction * compareFunction, Destructor * destructor) {
	return objectTypePropertiesNestedInit(toStringFunction, compareFunction, destructor, 0, 0);
}

ObjectType * objectTypePropertiesNestedInit(ToStringFunction * toStringFunction, CompareFunction * compareFunction, Destructor * destructor, int nestedTypesSize, int numProperties,  ...) {
	ObjectType * objType = malloc(sizeof(ObjectType));
	objType->toStringFunction = toStringFunction;
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
	if(objectType->destructor)
		objectType->destructor(objectType, value);
}

void objectTypePrintValue(ObjectType * objectType, void * value) {
	if(objectType->toStringFunction) {
		char * valueString = objectType->toStringFunction(objectType, value);
		printf("%s", valueString);
		free(valueString);
	}
}

char* objectTypeToString(ObjectType * objectType, void * value) {
	if(objectType->toStringFunction)
		return objectType->toStringFunction(objectType, value);
	return 0;
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
	if(objectType->compareFunction)
		return objectType->compareFunction(objectType, value1, value2);
	else
		return 0;
}

int objectTypeGetNestedTypesSize(ObjectType * objectType) {
	return objectType->nestedTypesSize;
}

int objectTypeGetPropertiesSize(ObjectType * objectType) {
	return objectType->numProperties;
}


bool objectTypeHasDestructor(ObjectType * objectType) {
	return objectType->destructor? true : false;
}

bool objectTypeHasCompareFunction(ObjectType * objectType) {
	return objectType->compareFunction? true : false;
}

bool objectTypeHasToStringFunction(ObjectType * objectType) {
	return objectType->toStringFunction? true : false;
}



ObjectType * objectTypeNULL() {
	if(!NULL_TYPE)
		NULL_TYPE = objectTypeInit(0, 0, 0);
	return NULL_TYPE;
}


















static char* CHAR_TO_STRING(ObjectType * objectType, void * value){
	char * temp = malloc(sizeof(char)*2);
	*temp = *(char *)value;
	*(temp+1) = '\0';
	return temp;
}

static int CHAR_COMPARE(ObjectType * objectType, void * value1, void * value2){
	int charValue1 = *(char *)value1;
	int charValue2 = *(char *)value2;
	if(charValue1 < charValue2)
		return -1;
	else if (charValue1 > charValue2)
		return 1;
	return 0;
}

static void CHAR_DESTRUCTOR(ObjectType * objectType, void * value) {
	free(value);
}

static char* STRING_TO_STRING(ObjectType * objectType, void * value){
	char * stringValue = (char*) value;
	char c = *stringValue;
	int size = getStringSize(stringValue);
	char * temp = malloc(sizeof(char) * (size+1));
	for(int i = 0; i < size; i++)
		*(temp+i) = *(stringValue+i);	
	*(temp+size) = '\0';

	return temp;
}

static int STRING_COMPARE(ObjectType * objectType, void * value1, void * value2){
	char * string1 = *(char **)value1;
	char * string2 = *(char **)value2;
	return strcmp(string1, string2);
}

static void STRING_DESTRUCTOR(ObjectType * objectType, void * value) {
	free(value);
}

static char* INT_TO_STRING(ObjectType * objectType, void * value){
	int intValue = *(int *)value;
	int size = 0;
	bool isNeg = false;
	if(intValue < 0) {
		isNeg = true;
		intValue  = intValue * -1;
		size++;
	}
	if(intValue != 0) {
		int currentValue = intValue;
		while (currentValue != 0) {
			currentValue /= 10;
			size++;
		}
	} else {
		size++;
	}

	char * temp = malloc(sizeof(char) * (size +1));
	int i = 0;
	if(isNeg) {
		*(temp) = '-';
		i++;
	}

	int currentValue = intValue;
	int moduloValue;
	for(; i < size; i++) {
		moduloValue = currentValue % 10;
		*(temp + i) = moduloValue;
		currentValue /= 10;
		i++;
	}
	*(temp+size) = '\0'; 
	return temp;
}

static int INT_COMPARE(ObjectType * objectType, void * value1, void * value2){
	int charValue1 = *(int *)value1;
	int charValue2 = *(int *)value2;
	if(charValue1 < charValue2)
		return -1;
	else if (charValue1 > charValue2)
		return 1;
	return 0;
}

static void INT_DESTRUCTOR(ObjectType * objectType, void * value) {
	free(value);
}