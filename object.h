#ifndef OBJECT_H
#define OBJECT_H

#include <stdbool.h>


typedef struct ObjectType ObjectType ;

// COMMON OBJECT TYPES:
extern ObjectType CHAR_OBJECT, STRING_OBJECT, INT_OBJECT;


typedef char* ObjectTypeToStringFunction(ObjectType * objectType, void * value);
typedef void ObjectTypeDestructor (ObjectType * objectType, void * value);
typedef int ObjectTypeCompareFunction (ObjectType * objectType, void * value1, void * value2);


ObjectType * objectTypeInit(ObjectTypeToStringFunction * toStringFunction, ObjectTypeCompareFunction * compareFunction, ObjectTypeDestructor * destructor);
ObjectType * objectTypePropertiesNestedInit(ObjectTypeToStringFunction * toStringFunction, ObjectTypeCompareFunction * compareFunction, ObjectTypeDestructor * destructor, int nestedTypesSize, int numProperties, ...);
void objectTypeDestroyTypeOnly(ObjectType * objectType);
void objectTypeDestroyValue(ObjectType * objectType, void * value);
void objectTypePrintValue(ObjectType * objectType, void * value);
char* objectTypeToString(ObjectType * objectType, void * value);
ObjectType * objectTypeGetNestedType(ObjectType * objectType, int index);
void * objectTypeGetProperty(ObjectType * objectType, int index);
int objectTypeCompare(ObjectType * objectType, void * value1, void * value2);
int objectTypeGetNestedTypesSize(ObjectType * objectType);
int objectTypeGetPropertiesSize(ObjectType * objectType);
bool objectTypeHasDestructor(ObjectType * objectType);
bool objectTypeHasCompareFunction(ObjectType * objectType);
bool objectTypeHasToStringFunction(ObjectType * objectType);

ObjectType * objectTypeNULL();

#endif // OBJECT_H