#include <stdio.h>
#include <stdbool.h>
#include "main.h"

void appendToBuffer(int number) {
    if(c.count<c.arraySize) {
        int insertionPoint = c.current+c.count;
        if(insertionPoint>=c.arraySize) {
            insertionPoint-=c.arraySize;
        }
        c.buffer[insertionPoint]=number;
        c.count++;
    } else if(c.count==c.arraySize) {
        c.buffer[c.current]=number;
        c.current=countCircular();
        printf("The buffer is full! I removed a part of the buffer to insert your integer!");
    }
}
