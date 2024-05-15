#include <stdio.h>

void printReferentie(int x){
	printf("Het adres van deze variabele is %p\n", &x);
}

void printWaardenEnAdressen(int *x, int *y){
	printf("x: %d\n",*x);
	printf("y: %d\n",*y);
	printf("x: ");
	printReferentie(*x);
	printf("y: ");
	printReferentie(*y);
}

void verwisselWaarden(int *p1, int *p2){
	int p3 = *p1;
	*p1 = *p2;
	*p2 = p3;	
}

int main(){
	int x = 10, y = 20;
	printReferentie(x);
	printWaardenEnAdressen(&x,&y);
	verwisselWaarden(&x,&y);
	printWaardenEnAdressen(&x,&y);
}
