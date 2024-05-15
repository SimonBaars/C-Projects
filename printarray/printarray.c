#include <stdio.h>
#define INT_ARRAY_SIZE(array) sizeof(array)/sizeof(int)

void printArray(int array[], int arraySize){
	for(int i = 0; i<arraySize; i++){
		printf("%d\n",array[i]);
	}
}

int main(){
	int array[] = {4,7,3,6,7,8,3,9};
	printArray(array, INT_ARRAY_SIZE(array));
}
