#pragma once
#include <stdbool.h>
#define SIZE 10

typedef struct {
        int arraySize;
        int current;
        int count;
        int buffer[SIZE];
    } CirculaireBuffer;
    
    CirculaireBuffer c;

void appendToBuffer(int number);
int readFromBuffer();
bool isEmpty();
int countCircular();
