// LedHandler.cpp

#include "LedHandler.h"

LedHandler::LedHandler(PinName pin, const bool activeHigh_)
    : led(pin),
      mode(LedMode::Manual),
      activeHigh(activeHigh_)
{
    setOff();
}

bool LedHandler::isOn()
{
    return (led.read() == (activeHigh ? 1 : 0));
}

LedHandler::LedMode LedHandler::getMode()
{
    return mode;
}

void LedHandler::setManual(const bool on)
{
    mode = LedMode::Manual;
    cicleTimeout.detach();
    if (on)
        setOn();
    else
        setOff();
}

void LedHandler::setOn()
{
    led = activeHigh ? 1 : 0;
}

void LedHandler::setOff()
{
    led = activeHigh ? 0 : 1;
}

void LedHandler::toggle()
{
    mode = LedMode::Manual;
    cicleTimeout.detach();
    led = !led;
}

//-------------------------------------------------

void LedHandler::setCicle(std::vector<float> cicleVector_, const bool oneTime_)
{
    if (cicleVector_.size() < 2)
    {
        return;
    }
    cicleVector = cicleVector_;
    cicleIndex = 0;
    oneTime = oneTime_;

    mode = LedMode::Cicle;

    setOn();

    cicleTimeout.attach(callback(this, &LedHandler::cicleCallback), cicleVector[0]);
}

void LedHandler::cicleCallback()
{
    cicleTimeout.detach();
    // siguiente estado
    cicleIndex++;

    // si llego al final
    if (cicleIndex >= cicleVector.size())
    {
        if (oneTime)
        {
            setManual(false);
            return;
        }
        else
        {
            cicleIndex = 0;
        }
    }

    // si cicleIndex.size % 2 == 0, encender
    if (cicleIndex % 2 == 0)
    {
        setOn();
    }
    else
    {
        setOff();
    }

    cicleTimeout.attach(callback(this, &LedHandler::cicleCallback), cicleVector[cicleIndex]);
}