#include "MPU6050.h"

void InitMPU6050(void)
{
	uint8_t buf[2];
	buf[0] = power_mgmt_1;
	buf[1] = 0;
	ret = HAL_I2C_Master_Transmit(&hi2c3, MPU6050, buf, 2, TIMEOUT);
		if(ret!=HAL_OK)
		{
			Error_Handler();
//			strcpy((char *) buf, "Error Rx\n\r");
		}
}

double lageAbfragen(uint8_t address) 
{
	
	uint8_t buf[2];
	int16_t beschleunigungsberechnung;
	double beschleunigung = 500000;
	buf[0] = address;
		ret = HAL_I2C_Master_Transmit(&hi2c3, MPU6050, buf, 1, TIMEOUT);
		if(ret!=HAL_OK)
		{
			//Error_Handler();
//			strcpy((char *) buf, "Error Rx\n\r");
		}
		else
		{
			ret = HAL_I2C_Master_Receive(&hi2c3, MPU6050, buf, 2,TIMEOUT);
			if(ret != HAL_OK)
			{
				//Error_Handler();
//				strcpy((char *) buf, "Error Tx\n\r");
			}
			else
			{
				int16_t upper = buf[0];
				uint8_t lower = buf[1];
				beschleunigungsberechnung = ((int16_t)upper *0x100 +(uint16_t)lower);
				beschleunigung = beschleunigungsberechnung/(double)16384;	
			}
		}
		return beschleunigung;
}

double berechneDistanz(double laenge, double breite)
{
	return(sqrt((laenge*laenge)+(breite*breite)));
}


void berechneRotation(double xBeschleunigung, double yBeschleunigung, double zBeschleunigung, double buf[])
{
	double xzBeschleunigung = berechneDistanz(xBeschleunigung, zBeschleunigung);
	double yzBeschleunigung = berechneDistanz(yBeschleunigung, zBeschleunigung);
	
	double yWinkelRad = atan2(xBeschleunigung, yzBeschleunigung);
	double xWinkelRad = atan2(yBeschleunigung, xzBeschleunigung);
	double xWinkelDeg = xWinkelRad*180/pi;
	double yWinkelDeg = yWinkelRad*180/pi;
	buf[0] = xWinkelDeg;
	buf[1] = yWinkelDeg;
}


bool foehnWinkelBerechnung(double drehungX, double drehungY)
{
	if((drehungX > minDrehungX && drehungX < maxDrehungX) && (drehungY > minDrehungY && drehungY < maxDrehungY))
	{
		return 0;
	}
	else // Gestell fallt um
	{
		return 1;
	}
}

void foehnlageberechnung(double *rotation) // Buffer leange von 5, um auch x,y,z wegen debugging abzufragen
{
	const uint8_t ACCEL_XOUT = 0x3B;
	const uint8_t ACCEL_YOUT = 0x3D;
	const uint8_t ACCEL_ZOUT = 0x3F;
	
	static double x = 0; 
	static double y = 0; 
	static double z = 10;
	double new_val = 0;
	static uint8_t counter = 0;
	
	if(counter == 0)
	{
			new_val = lageAbfragen(ACCEL_XOUT); // Wert[0] im Array Rotation
		if (new_val != 500000)
		{
			x = new_val;
		}
			rotation[0] = x;
	}
	else if(counter == 1)
	{
			new_val = lageAbfragen(ACCEL_YOUT); //Wert [1] im Array Rotation
				if (new_val != 500000)
		{
			y = new_val;
		}
			rotation[1] = y;
	}
	else if(counter == 2)
	{
			new_val = lageAbfragen(ACCEL_ZOUT);	//Wert[2] im Array Rotation
				if (new_val != 500000)
		{
			z = new_val;
		}
			rotation[2] = z;
	}
	
	++counter;	
	if (counter > 2)		// soll x, y, z Werte abfragen, dann von vorne anfangen
	{
		counter = 0;
	}
	
	berechneRotation(x, y, z, &rotation[3]); // [3] WinkelX, [4]WinkelY
}
