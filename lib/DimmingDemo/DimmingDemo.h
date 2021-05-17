/**
 * @file DimmingDemo.h
 * @author Agustin Allegui (a.allegui@gmail.com)
 * @brief Modo demostracion de nivel de dimming para controlador de luz
 * @version 0.1
 * @date 2020-09-11
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef DIMMING_DEMO_H
#define DIMMING_DEMO_H

#include <stdint.h>

#define DIMMING_DEMO_INCREMENT 0.2 // Salto de nivel de dimming.
#define DIMMING_DEMO_PERIOD 60     // Segundos entre cambios de dimming en el modo demo.

class DimmingDemo
{
public:
    DimmingDemo();

    /**
     * @brief Obtiene el nivel de dimming para mostrar ahora.
     * 
     * @return valor de dimming (0 - 1)
     */
    float getDimming();

    static const uint16_t demoPeriod; // Segundos entre cambios de dimming en el modo demo.

protected:
    bool goingUp;
    float lastValue;
};

#endif