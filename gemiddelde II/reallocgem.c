#include <stdio.h>
#include <stdlib.h>

float bepaalGemiddelde(int array[], int arraySize){
	int totaal = 0;
	for(int i = 0; i<arraySize; i++){
		totaal+=array[i];
	}
	return totaal/arraySize;
}

void printArray(int array[], int arraySize){
	for(int i = 0; i<arraySize; i++){
		printf("%d\n",array[i]);
	}
}

int clean_stdin() {
    while(getchar()!='\n');
    return 1;
}

int main() {
    char ch;
    int *array = malloc(sizeof(int));
    int i = 0;
    while(1){
        do {
            printf("\nVoer cijfer %d in: (of 0 om te stoppen met invoeren)", i+1);
        } while(((scanf("%d%c", &array[i], &ch)!=2 || ch!='\n') && clean_stdin()) || array[i]<0 || array[i]>999999);
        if(array[i]==0){
			break;
		}
        i++;
        array=realloc(array,i*sizeof(int));
	}
    printArray(array,i);
    printf("Het gemiddelde van deze cijfers is %f\n",bepaalGemiddelde(array,i));
    free(array);
    return 0;
}
