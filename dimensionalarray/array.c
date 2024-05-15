#include <stdio.h>
#include <stdlib.h>

int bepaalHoogste(int array[], int arraySize){
	int hoogste = array[0];
	for(int i = 1; i<arraySize; i++){
		if(array[i]>hoogste){
			hoogste=array[i];
		}
	}
	return hoogste;
}

void free_data(int ***data, size_t xlen, size_t ylen)
{
    size_t i, j;

    for (i=0; i < xlen; ++i) {
        if (data[i] != NULL) {
            for (j=0; j < ylen; ++j)
                free(data[i][j]);
            free(data[i]);
        }
    }
    free(data);
}

int ***alloc_data(size_t xlen, size_t ylen, size_t zlen)
{
    int ***p;
    size_t i, j;

    if ((p = malloc(xlen * sizeof *p)) == NULL) {
        perror("malloc 1");
        return NULL;
    }

    for (i=0; i < xlen; ++i)
        p[i] = NULL;

    for (i=0; i < xlen; ++i)
        if ((p[i] = malloc(ylen * sizeof *p[i])) == NULL) {
            perror("malloc 2");
            free_data(p, xlen, ylen);
            return NULL;
        }

    for (i=0; i < xlen; ++i)
        for (j=0; j < ylen; ++j)
            p[i][j] = NULL;

    for (i=0; i < xlen; ++i)
        for (j=0; j < ylen; ++j)
            if ((p[i][j] = malloc(zlen * sizeof *p[i][j])) == NULL) {
                perror("malloc 3");
                free_data(p, xlen, ylen);
                return NULL;
            }

    return p;
}

int main(){
	int ***array = alloc_data(4,5,6);
	for(int i = 0; i<4; i++){
		for(int j = 0; j<5; j++){
			for(int k = 0; k<6; k++){
				array[i][j][k]=rand() % 20;
			}
			printf("Hoogste van array %d, %d is %d\n", i, j, bepaalHoogste(array[i][j],6));
		}
	}
	free_data(array, 4, 5);
	return 0;
}
