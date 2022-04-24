#ifndef FIR_FILTER_H
#define FIR_FILTER_H

#include <stdint.h>

#define FIR_FILTER_LENGTH 10

typedef struct 
{
	double 	buf[FIR_FILTER_LENGTH];
	uint8_t bufIndex;
	double cummulated_value;

	double out;
	
} FIRFilter;

extern void FIRFilter_Init(FIRFilter *fir);
extern double FIRFilter_Update(FIRFilter *fir, double inp);

#endif
