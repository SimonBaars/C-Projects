#include <stdio.h>
#include <stdbool.h>
#include "servo.h"
#include "printservo.h"

void printServos();
bool conflict();
bool servosMoving();

int main() {
    Servo mijnServo[]= {
        {1,5,45.0,idle},
        {2,12,8.5,moving},
        {3,12,9.5,idle}
    };
    int arraysize = 3;
    printServos(mijnServo, arraysize);
    printf("conflict=%d\n",conflict(mijnServo, arraysize));
    return 0;
}


void printServos(Servo s[], int arraysize) {
    int i;
    for(i=0; i<arraysize; i++) {
        printServo(s[i]);
        printf("-----------------------\n");
    }
}

bool servosMoving(Servo s[], int arraysize) {
    int i;
    for(i=0; i<arraysize; i++) {
        if(servoMoving(s[i])) {
            return true;
        }
    }
    return false;
}

bool conflict(Servo s[], int arraysize){
	int i;
	int j;
	for(i = 0; i<arraysize; i++){
		for(j=0; j<arraysize; j++){
			if(i!=j && (s[i].pin==s[j].pin || s[i].id==s[j].id)){
				return true;
			}
		}
	}
	return false;
}
