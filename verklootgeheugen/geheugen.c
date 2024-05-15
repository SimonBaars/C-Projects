#include <stdio.h>

int main(){
	int j = 2;
	for(int i = 0; i<1000000; i++){
		*(&j+i)=i;
		printf("%d geheugenadressen verkloot\n", i);
	}
}
