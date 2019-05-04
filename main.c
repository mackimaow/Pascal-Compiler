#include "stdio.h"
#include "object.h"

void test(int i, int j, int * k) {
	printf("%d\n", *k);
	printf("%d\n", *(k+1));
	printf("%d\n", *(k+2));
}


int main() {
	
}