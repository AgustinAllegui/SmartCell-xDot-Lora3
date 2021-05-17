// DimmingCurves.cpp

#include "DimmingCurves.h"

const uint8_t DimmingCurves::curves[][12] = {
    //  0     2     4     6     8     10    12    14    16    18    20    22
    // 1     3     5     7     9     11    13    15    17    19    21    23
    {0x46, 0x44, 0x44, 0xA6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xAA, 0x8A}, // DC1 Residencial
    {0x33, 0x33, 0x33, 0xA5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xAA, 0x58}, // DC Plazas
    {0x55, 0x55, 0x55, 0xA8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xAA, 0x88}, // DC Avenidas
};

DimmingCurves::DimmingCurves(const uint8_t index)
    : currentCurve(index)
{
  staticCurves = sizeof(curves) / sizeof(curves[0]);
  selectCurve(index);
}

uint8_t DimmingCurves::selectCurve(const uint8_t index)
{
  if (index > staticCurves)
  {
    currentCurve = 0;
  }
  else
  {
    currentCurve = index;
  }
  return currentCurve;
}

float DimmingCurves::getDimming(const uint8_t hour)
{
  if (hour > 23)
  {
    // escapar si hora mayor a 23
    return 0;
  }

  uint8_t twoValues;
  if (currentCurve < staticCurves)
  {
    twoValues = curves[currentCurve][hour / 2];
  }
  else if (currentCurve == staticCurves)
  {
    twoValues = customCurve[hour / 2];
  }

  uint8_t dimming;
  if (hour % 2 != 0)
  {
    dimming = twoValues & 0xF0;
    dimming = dimming >> 4;
  }
  else
  {
    dimming = twoValues & 0x0F;
  }

  return (static_cast<float>(dimming) / 10);
}
