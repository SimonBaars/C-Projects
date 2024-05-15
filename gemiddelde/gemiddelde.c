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
    int nCijfers = 0;
    char ch;
    do {
        printf("\nVan hoeveel cijfers wilt u het gemiddelde bepalen: ");
    } while(((scanf("%d%c", &nCijfers, &ch)!=2 || ch!='\n') && clean_stdin()) || nCijfers<0 || nCijfers>10);
    int *array = malloc(nCijfers*sizeof(int));
    for(int i = 0; i<nCijfers; i++) {
        do {
            printf("\nVoer cijfer %d in: ", i+1);
        } while(((scanf("%d%c", &array[i], &ch)!=2 || ch!='\n') && clean_stdin()) || array[i]<0 || array[i]>999999);
    }
    printArray(array,nCijfers);
    printf("Het gemiddelde van deze cijfers is %f\n",bepaalGemiddelde(array,nCijfers));
    free(array);
    return 0;
}
