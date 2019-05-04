#include <stdlib.h>
#include "utils.h"

ListPrintProperties defaultPrintProperties = {"[ ",", "," ]"};
MapPrintProperties defaultMapPrintProperties = {"( "," : "," )", &defaultPrintProperties};
TreePrintProperties defaultTreePrintProperties = {"[", "]", " |", "__________"};

void resetOptions (ForEachOptions * options) {
	options->remove = false;
	options->insert = 0;
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