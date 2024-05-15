#include <stdio.h>

int main() {
    short i;
    short prevChar;
    for(i = 1; i>-1; i=((i+1)*2)-1) {
        if(prevChar==i) {
            break;
        }
        printf( "%d\n", i );
        prevChar=i;
    }
    return 0;
}
