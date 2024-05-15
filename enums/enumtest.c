#include <stdio.h>

typedef enum { man, vrouw } geslacht;
geslacht geslachtVanPietje=man;

int main(){
	printf("%d\n", geslachtVanPietje);
	return 0;
}
