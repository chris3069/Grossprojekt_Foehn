/* ---------------------------------------------------------
Funktionen für servo Motor "SunFounder 
--------------------------------------------------------- */

#include "stm32f4xx_hal.h"
#include <stdio.h>
#include "servo.h"
#include "tim.h"


void startPWM2 ()
{
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
}


void servoReset()
{
	TIM2->CCR2 = 150;
}


void servoDegtoDeg(int degLeft, int degRight)
{
	
	double actVal = TIM2->CCR2;
	int valLeft = 0;
	int valRight = 0;
	static unsigned int rotBool = 0;
	
	valLeft = mapfun(degLeft);
	valRight = mapfun(degRight);
	
	
	if(degLeft < 5 || degRight > 265)					//Winkelbreich festlegen
	{
		return;
	}
	
	if(degLeft >= degRight)									// Überprüfen das der Rechte Winkel größer als der kleine ist 
	{
		return;
	}
	
	valLeft = mapfun(degLeft);
	valRight = mapfun(degRight);
	
		if(rotBool == 0)
	{
		actVal--;
		TIM2->CCR2 = actVal;	
		if(actVal < (double) valLeft)
		{
			rotBool = 1;
		}	
	}
	
	if(rotBool == 1)
	{
		actVal++;
		TIM2->CCR2 = actVal;	
		if(actVal > (double) valRight)
		{
			rotBool = 0;
		}	
	}
}




int mapfun(int deg)
{

	int retVal = 0;
	
	retVal = deg*200/270 + 50;
	
	return retVal;
}



//void createTable(double *table)
	


