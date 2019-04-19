#include <stdlib.h>

typedef struct { int * num; } Test;

int main() {
	Test *test = (Test*) malloc(sizeof(Test));
}
