#include <stdio.h>

int main() {
    float i;
    float prevChar;
    for(i = 1; i>-1; i=((i+1)*2)-1) {
        if(prevChar==i) {
            break;
        }
        printf( "%f\n", i );
        prevChar=i;
    }
    return 0;
}
