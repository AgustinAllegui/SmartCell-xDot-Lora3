// PulseCounter.cpp

#include "PulseCounter.h"

/*	
 */
PulseCounter::PulseCounter(PinName pin, const bool activeHigh, const bool filter, const uint8_t divisor)
	: _interrupt(pin), _activeHigh(activeHigh), _div(divisor), _filtering(false)
{
	// create the InterruptIn on the pin specified to Counter
	if (activeHigh)
	{
		// attach increment/filter method of this counter instance
		_interrupt.rise(callback(this, filter ? (&PulseCounter::filterInit) : (&PulseCounter::increment))); 
	}
	else
	{
		// attach increment/filter method of this counter instance
		_interrupt.fall(callback(this, filter ? (&PulseCounter::filterInit) : (&PulseCounter::increment))); 
	}
	_count = 0;
}

void PulseCounter::filterInit()
{
	if (_filtering)
	{
		return;
	}
	_filtering = true;
	_timeout.attach_us(callback(this, &PulseCounter::filterCall), DEBUNCE_TIMEOUT_US);
}

void PulseCounter::filterCall()
{
	// if interrupt pin not in active state, scape from filter
	if (_interrupt.read() == _activeHigh ? 1 : 0)
	{
		increment();
	}
	_filtering = false;
	_timeout.detach();
}

void PulseCounter::increment()
{
	_count++;
}

uint16_t PulseCounter::read()
{
	return static_cast<uint8_t>(_count / _div);
}

void PulseCounter::reset()
{
	_count = 0;
}
