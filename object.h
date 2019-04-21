#ifndef OBJECT_H
#define OBJECT_H

typedef struct ObjectType ObjectType ;

typedef void Destructor (ObjectType * objectType, void * value);
typedef int CompareFunction (ObjectType * objectType, void * value1, void * value2);
typedef void PrintFunction(ObjectType * objectType, void * value);

ObjectType * objectTypeInit(PrintFunction * printFunction, CompareFunction * compareFunction, Destructor * destructor);
ObjectType * objectTypePropertiesNestedInit(PrintFunction * printFunction, CompareFunction * compareFunction, Destructor * destructor, int nestedTypesSize, int numProperties, ...);
void objectTypeDestroyTypeOnly(ObjectType * objectType);
void objectTypeDestroyValue(ObjectType * objectType, void * value);
void objectTypePrintValue(ObjectType * objectType, void * value);
ObjectType * objectTypeGetNestedType(ObjectType * objectType, int index);
void * objectTypeGetProperty(ObjectType * objectType, int index);
int objectTypeCompare(ObjectType * objectType, void * value1, void * value2);
int objectTypeGetNestedTypesSize(ObjectType * objectType);
int objectTypeGetPropertiesSize(ObjectType * objectType);

#endif // OBJECT_H