#include <stdbool.h>
#include "main.h"

int countCircular() {
    if(c.current+1<c.arraySize) {
        return c.current+1;
    }
    return 0;
}

bool isEmpty() {
    if(c.count==0) {
        return true;
    }
    return false;
}

