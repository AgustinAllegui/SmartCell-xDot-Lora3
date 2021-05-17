/**
 * @file DimmingDemo.cpp
 * @author Agustin Allegui (a.allegui@gmail.com)
 * @brief Modo demostracion de nivel de dimming para controlador de luz
 * @version 0.1
 * @date 2020-09-11
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "DimmingDemo.h"

const uint16_t DimmingDemo::demoPeriod = DIMMING_DEMO_PERIOD;

DimmingDemo::DimmingDemo()
    : goingUp(false),
      lastValue(1)
{
}

float DimmingDemo::getDimming()
{
    if (goingUp)
    {
        lastValue += DIMMING_DEMO_INCREMENT;
        if (lastValue >= 1)
        {
            lastValue = 1;
            goingUp = false;
        }
    }
    else
    {
        lastValue -= DIMMING_DEMO_INCREMENT;
        if (lastValue <= 0.2)
        {
            lastValue = 0.2;
            goingUp = true;
        }
    }
    return lastValue;
}