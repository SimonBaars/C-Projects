#include <stdio.h>

void printLengteVanPointerinhoud(int *x){
	printf("Lente van de pointerinhoud van \"array\": %d\n", sizeof(*x)/sizeof(int));
}

int main(){
	int array[] = {4,7,3,6,7,8,3,9};
	printf("Lente van \"array\": %d\n", sizeof(array)/sizeof(int));
	printLengteVanPointerinhoud(array);
}
