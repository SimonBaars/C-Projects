#include <stdio.h>
#include <stdlib.h>

int main(){
	unsigned long x = 1;
	while(1){
		int *i = malloc(x*sizeof(int));
		printf("%lu\n",x*sizeof(int));
		if(i==NULL){
			break;
		}
		x+=1000;
		free(i);
	}
}
