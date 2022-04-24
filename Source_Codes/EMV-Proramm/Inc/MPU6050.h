#ifndef __MPU6050__
#define __MPU6050__

//#ifdef __cplusplus
//extern "C" {
//#endif
	
	#include <stdint.h>
	#include "i2c.h"


static const uint8_t power_mgmt_1 = 0x6B;
static const uint8_t power_mgmt_2 = 0x6C;

static const uint8_t MPU6050 = 0xD0; // 7 Bit Adresse bereits

static const double pi = 3.14159265358979323846;

static const int8_t maxDrehungX = 10; 
static const int8_t maxDrehungY = 10; 
static const int8_t minDrehungX = -10; 
static const int8_t minDrehungY = -10; 

static HAL_StatusTypeDef ret;

extern void InitMPU6050(void);
extern double lageAbfragen(uint8_t address);

extern void foehnlageberechnung(double *rotation); // Buffer leange von 5, um auch x,y,z wegen debugging abzufragen
extern bool foehnWinkelBerechnung(double, double);


//#ifdef __cplusplus
//}
//#endif

#endif /* __MPU6050__ */
