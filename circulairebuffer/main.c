#include <stdio.h>
#include <stdbool.h>
#include "main.h"

int clean_stdin() {
    while(getchar()!='\n');
    return 1;
}

int main() {
    bool mode = false;
    c.arraySize=10;
    c.current=0;
    c.count=0;
    while(1) {
        if(mode==false) {
            int i = 0;
            char ch;
            do {
                printf("\nAdd to buffer an integer from 1 to 100000: (insert 0 to change mode)");
            } while(((scanf("%d%c", &i, &ch)!=2 || ch!='\n') && clean_stdin()) || i<0 || i>100000);
            if(i==0) {
                mode=true;
                continue;
            }
            appendToBuffer(i);
        } else {
            printf("\nPress ENTER to get an integer from the buffer (insert 0 to change mode)");
            if(getchar()=='0') {
                mode=false;
                continue;
            }
            if(isEmpty()==false) {
                printf("%d\n", readFromBuffer());
            } else {
                printf("It's empty");
                mode = false;
            }
        }
    }

    return 0;
}
