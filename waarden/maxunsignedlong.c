#include <stdio.h>

int main() {
    unsigned long i;
    unsigned long prevChar=-1;
    for(i = 1; i==i; i=((i+1)*2)-1) {
        if(prevChar==i) {
            break;
        }
        printf( "%lu\n", i );
        prevChar=i;
    }
    return 0;
}
