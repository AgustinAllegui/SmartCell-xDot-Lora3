// CurrentSensor.cpp

#include "CurrentSensor.h"

CurrentSensor::CurrentSensor(PinName pin)
    : currentIn(pin)
{
}

/*
*/
float CurrentSensor::getCurrent()
{
  float maximo = 0.0;
  float minimo = 1.0;
  float current = 0;

  timer.reset();
  timer.start();

  while (timer.read_ms() < 40)
  {
    current = ((currentIn.read()) - 0.5);
    if (current < minimo)
      minimo = current;
    if (current > maximo)
      maximo = current;
  }

  timer.stop();

  current = (maximo - minimo) * curr_sens * 3.3;
  if (current < thr_curr)
    current = 0;

  return current; //current in Ampere
}

float CurrentSensor::getCurrent(const uint8_t points)
{
  float average = 0;
  for (uint8_t i = 0; i < points; i++)
  {
    average += getCurrent();
  }
  return average / points;
}