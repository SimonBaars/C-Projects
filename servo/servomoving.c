#include <stdbool.h>
#include "servo.h"

bool servoMoving(Servo s){
	if(s.state==0){
		return false;
	}
	return true;
}
