#include <stdbool.h>
#include "main.h"

int readFromBuffer() {
    int number = c.buffer[c.current];
    c.current=countCircular();
    c.count--;
    return number;
}
