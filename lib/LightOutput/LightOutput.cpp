// LightOutput.cpp

#include "LightOutput.h"

const float LightOutput::dimmingFactor = 0.95;

LightOutput::LightOutput(PinName relayPin, PinName dimmingOutputPin)
    : relay(relayPin), dimmingOutput(dimmingOutputPin)
{
  dimmingOutput.period_us(1000);
  setOutput(0.0f);
}

void LightOutput::setOutput(const float dimmingValue_)
{
  dimmingValue = dimmingValue_;
  if (dimmingValue >= 1.0f)
  {
    dimmingValue = 1.0f;
  }
  if (dimmingValue < 0.0f)
  {
    dimmingValue = 0.0f;
  }

	
  dimmingOutput.write(dimmingValue * dimmingFactor);

  relay = (dimmingValue > 0.0f) ? 1 : 0;
}

uint8_t LightOutput::getDimmingValue()
{
  return dimmingValue;
}