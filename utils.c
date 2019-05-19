#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "utils.h"

ListPrintProperties defaultPrintProperties = {"[ ",", "," ]"};
MapPrintProperties defaultMapPrintProperties = {"( "," : "," )", &defaultPrintProperties};
TreePrintProperties defaultTreePrintProperties = {"[", "]", " |", "__________"};


/* ----------------------------------------------------------------------------- 
 * hashpjw
 * Peter J. Weinberger's hash function 
 * Source: Aho, Sethi, and Ullman, "Compilers", Addison-Wesley, 1986 (page 436).
 */
int hashpjw( void* value ) {
	char *s = (char*)value; 
	char *p; 
	unsigned h = 0, g; 
	
	for ( p = s; *p != EOS; p++ ) { 
		h = (h << 4) + (*p); 
		if ( g = h & 0xf0000000 ) { 
			h = h ^ ( g >> 24 ); 
			h = h ^ g; 
		} 
	} 
	return h % TABLE_SIZE; 
}


void resetOptions (ForEachOptions * options) {
	options->remove = false;
	options->insert = 0;
}


char * intToString(int integer) {
	if(integer == 0)
		return copyString("0");
	int totalSize = 0;
	int tempValue = integer;
	bool neg = tempValue < 0;
	if(neg) {
		tempValue *= -1;
		totalSize++;
	}
	while(tempValue != 0) {
		tempValue /= 10;
		totalSize++;
	}
	char * temp = malloc(sizeof(char) * (totalSize + 1));
	sprintf(temp, "%i", integer);
	return temp;
}

int getStringSize(char * stringValue) {
	if(!stringValue)
		return -1;

	int size = 0;
	char c = *stringValue;
	while (c != '\0') {
		size++;
		c = *(stringValue + size);
	}
	return size;
}

int stringToInt(char * value) {

	int firstHalf = 0;

	int index = 0;
	char c = value[0];
	while (c != 'E' && c != '\0') {
		firstHalf = firstHalf * 10 + c - '0';
		index ++;
		c  = value [index]; 
	}
	if ( c == 'E' ) {
		bool negate = false;
		index++;
		c = value[index];
		if (c == '+') {
			index++;
			c = value[index];
		} else if(c == '-') {
			negate = true;
			index++;
			c = value[index];
		}
		int secondHalf = 0;
		while ( c != '\0' ) {
			secondHalf = secondHalf * 10 + c - '0';
			index ++;
			c  = value [index]; 
		}
		if (negate)
			secondHalf *= -1;
		secondHalf = pow(10, secondHalf);
		return firstHalf * secondHalf;
	} else {
		return firstHalf;
	}
}


void stringInsert(char * totalString, char * insertString, int startIndex) {
	int i = 0;
	char c = *insertString;
	while(c != '\0') {
		*(totalString + i + startIndex) = c;
		i++;
		c = *(insertString+i);
	}
}

char * copyString(char * stringValue) {
	int stringSize = getStringSize(stringValue);
	char * temp = malloc(sizeof(char)*(stringSize+1));
	stringInsert(temp, stringValue, 0);
	temp[stringSize] = '\0';
	return temp;
}

char * stringTakeLast(char * totalString, int startInclusive) {
	int stringSize = getStringSize(totalString);
	if(startInclusive < 0 || startInclusive > stringSize)
		return 0;
	int newSize = stringSize - startInclusive;
	char * lastPart = malloc(sizeof(char) * (newSize + 1));
	for(int i = 0; i < newSize; i++)
		lastPart[i] = totalString[startInclusive+i];	

	lastPart[newSize] = '\0';
	return lastPart;
}

char * stringTakeSubstring(char * totalString, int startInclusive, int endExclusize) {
	int stringSize = getStringSize(totalString);
	if(startInclusive < 0 || endExclusize > stringSize || startInclusive > endExclusize)
		return 0;
	int newSize = endExclusize - startInclusive;
	char * lastPart = malloc(sizeof(char) * (newSize + 1));
	for(int i = 0; i < newSize; i++)
		lastPart[i] = totalString[startInclusive+i];	

	lastPart[newSize] = '\0';
	return lastPart;
}





