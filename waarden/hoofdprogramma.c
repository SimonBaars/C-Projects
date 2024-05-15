#include <stdio.h>

int main() {
    int i;
    for(i = 1; i>-1; i=((i+1)*2)-1) {
        printf( "%d\n", i );

    }
    return 0;
}
