#ifndef __SERVO_H__
#define __SERVO_H__

//#ifdef __cplusplus
//extern "C" {
//#endif
	
#include <stdio.h>
#include "servo.h"
#include "tim.h"
	
void servoDegtoDeg(int degLeft, int degRight);
int mapfun(int deg);
void startPWM2 (void);
void servoReset(void);


//#ifdef __cplusplus
//}
//#endif

#endif /* __MPU6050__ */
