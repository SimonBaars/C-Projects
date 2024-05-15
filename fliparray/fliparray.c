#include <stdio.h>
#define INT_ARRAY_SIZE(array) sizeof(array)/sizeof(int)

void printArray(int array[], int arraySize){
	for(int i = 0; i<arraySize; i++){
		printf("%d\n",array[i]);
	}
}

void verwisselWaarden(int *p1, int *p2){
	int p3 = *p1;
	*p1 = *p2;
	*p2 = p3;	
}

void flipArray(int array[], int arraySize){
	for(int i = 0; i<arraySize/2; i++){
		verwisselWaarden(&array[i],&array[arraySize-1-i]);
	}
}

int main(){
	int array[] = {1,2,3,4,5,6,7,8};
	printf("Array voor flip:\n");
	printArray(array, INT_ARRAY_SIZE(array));
	flipArray(array, INT_ARRAY_SIZE(array));
	printf("Array na flip:\n");
	printArray(array, INT_ARRAY_SIZE(array));
}
