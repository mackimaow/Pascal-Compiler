#include "stdio.h"


void test(int i, int j, int * k) {
	printf("%d\n", *k);
	printf("%d\n", *(k+1));
	printf("%d\n", *(k+2));
}


int main() {
	int thing[3] = {1, 2, 3};
	test(0, 1, thing);
}