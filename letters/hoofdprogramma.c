#include  <stdio.h>

void upperLowerCast(char c){
	char *check;
	if(c>='A' && c<='Z'){
		check="Upper";
	} else if (c>='a' && c<='z'){
		check="Lower";
	} else {
		check="Other";
	}
	printf("'%c': " , c );
	printf("%s\n",check);
}

int main(){
	upperLowerCast('d');
	upperLowerCast('X');
	upperLowerCast('.');
	return 0;
}
