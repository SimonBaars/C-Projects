#pragma once

typedef enum {idle,moving} ServoState;

typedef struct {
    int id;
    int pin;
    float angle;
    ServoState state;
} Servo;

