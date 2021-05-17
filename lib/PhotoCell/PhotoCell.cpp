// PhotoCell.cpp

#include "PhotoCell.h"

PhotoCell::PhotoCell(PinName pin_, const float onThreshold_, const float offThreshold_)
    : pin(pin_), lastState(false), lastRead(0)
{
  setThreshold(onThreshold_, offThreshold_);
}

void PhotoCell::setThreshold(const float onThreshold_, const float offThreshold_)
{
  onThreshold = onThreshold_;
  if (onThreshold_ <= offThreshold_)
  {
    offThreshold = offThreshold_;
  }
  else
  {
    offThreshold = onThreshold_;
  }
}

/*
 *  Lee el valor la cantidad de luz en el ambiente
 *  devuelve un valor entre 1 y 0.
 *  1 es mucha luz,
 *  2 es oscuridad
 *  Realiza un primedio de n puntos
 */
float PhotoCell::read(const uint8_t ammount)
{
  float accumulator = 0;
  for (uint8_t i = 0; i < ammount; i++)
  {
    float value = pin.read();
    //logDebug("-Valor medido %f", value);
    accumulator += (value / ammount);
  }

  lastRead = accumulator;
  return accumulator;
}

float PhotoCell::getLastRead()
{
  return lastRead;
}

/*
 * Devuelve true si la fotocelda indica 
 * que deberia encenderse la luz.
 */

//! dar vuelta. prende cuando hay luz
bool PhotoCell::shouldBeOn()
{
  float currentRead = read(10);
  //logDebug("-Promedio %f", currentRead);
  if (lastState)
  {
    if (currentRead > offThreshold)
    {
      lastState = false;
      return false;
    }
    else
    {
      lastState = true;
      return true;
    }
  }
  else
  {
    if (currentRead < onThreshold)
    {
      lastState = true;
      return true;
    }
    else
    {
      lastState = false;
      return false;
    }
  }
}
