#include <stdio.h>

void printReferentie(int *x){
	printf("Het adres van deze variabele is %p\n", x);
}

int main(){
	int *x = NULL;
	printReferentie(x);
	if(x==NULL){
		printf("Check succesful");
	} else {
		printf("Check unsuccesful");
	}
	*x = *x + 1;
	printReferentie(x);
}
