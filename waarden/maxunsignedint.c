#include <stdio.h>

int main() {
    unsigned int i;
    unsigned int prevChar=-1;
    for(i = 1; i==i; i=((i+1)*2)-1) {
        if(prevChar==i) {
            break;
        }
        printf( "%u\n", i );
        prevChar=i;
    }
    return 0;
}
