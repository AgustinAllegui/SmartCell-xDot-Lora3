// LightOutput.h

#ifndef _LIGHT_OUTPUT_H_
#define _LIGHT_OUTPUT_H_

#include "mbed.h"

class LightOutput
{
public:
  LightOutput(PinName relayPin, PinName dimmingOutputPin);
  void setOutput(const float dimmingValue_);

  // Getters

  uint8_t getDimmingValue();

protected:
  DigitalOut relay;
  PwmOut dimmingOutput;

  float dimmingValue;

private:
  const static float dimmingFactor;
};

#endif
