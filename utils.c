#include "utils.h"

ListPrintProperties defaultPrintProperties = {"[ ",", "," ]"};
MapPrintProperties defaultMapPrintProperties = {"( "," : "," )", &defaultPrintProperties};

void resetOptions (ForEachOptions * options) {
	options->remove = false;
	options->insert = 0;
}
