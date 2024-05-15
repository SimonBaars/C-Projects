#include <stdio.h>
#include <stdbool.h>
#include "servo.h"


void printServo(Servo s) {
    printf("id=%d\n",s.id);
    printf("pin=%d\n",s.pin);
    printf("angle=%.2f\n",s.angle);
    printf("state=%d\n",s.state);
}
