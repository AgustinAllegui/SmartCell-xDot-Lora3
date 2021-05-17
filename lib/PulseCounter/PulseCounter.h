// PulseCounter.h

#ifndef _PULSE_COUNTER_H_
#define _PULSE_COUNTER_H_

#include "mbed.h"

#define DEBUNCE_TIMEOUT_US 500

class PulseCounter
{
public:
	PulseCounter(PinName pin, const bool activeHigh = true, const bool filter = false, const uint8_t divisor = 1);
	uint16_t read();
	void reset();

private:
	InterruptIn _interrupt;
	bool _activeHigh;
	volatile uint16_t _count;
	unsigned int _div;

	bool _filtering;
	LowPowerTimeout _timeout;
	void filterInit();
	void filterCall();

	void increment();
};

#endif
