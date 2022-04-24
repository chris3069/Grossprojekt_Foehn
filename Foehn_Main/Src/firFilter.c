#include "firFilter.h"


void FIRFilter_Init(FIRFilter *fir) 
	{
	/* Clear filter buffer */
	for (uint8_t n = 0; n < FIR_FILTER_LENGTH; n++) 
		{
		fir->buf[n] = 0.0f;
		}

		fir->cummulated_value = 0;
	/* Reset buffer index */
	fir->bufIndex = 0;

	/* Clear filter output */
	fir->out = 0.0f;

}

double FIRFilter_Update(FIRFilter *fir, double inp) {
// Moving Average Filter
	
 	fir->cummulated_value -= fir->buf[fir->bufIndex];
	
	/* Store latest sample in buffer */
	fir->buf[fir->bufIndex] = inp;
	
	fir->cummulated_value += fir->buf[fir->bufIndex];

	/* Increment buffer index and wrap around if necessary */
	fir->bufIndex = fir->bufIndex +1;

	if (fir->bufIndex == FIR_FILTER_LENGTH) {

		fir->bufIndex = 0;

	}
	
fir->out = fir->cummulated_value / FIR_FILTER_LENGTH;
	/* Return filtered output */
	return fir->out;
	
}
