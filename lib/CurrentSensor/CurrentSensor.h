/**
 * @file CurrentSensor.h
 * @author Agustin Allegui (a.allegui@gmail.com)
 * @brief Sensor de corriente para nodo SmartCell-xDot
 * @version 0.1
 * @date 2020-09-13
 * 
 * @copyright Copyright (c) 2020
 * 
 */

/*	sensor de corriente de nodo SmartCell-xdot (desconozco el modelo del chip)
 */

#ifndef _CURRENT_SENSOR_H_
#define _CURRENT_SENSOR_H_

#include "mbed.h"

const float curr_sens = 1.352; // I/Vpp [mA/mV = A/V = 1/ohm]
const float thr_curr = .15;	   // zona muerta del sensor [Ampere]

class CurrentSensor
{
public:
	/**
	 * @brief Construct a new Current Sensor object.
	 * 
	 * @param pin Pin al que esta conectado el sensor.
	 */
	CurrentSensor(PinName pin);

	/**
	 * @brief Medir la corriente.
	 * 
	 * @return Corriente en amperes.
	 */
	float getCurrent();

	/**
	 * @brief Obtener un promedio de mediciones de corriente
	 * 
	 * @param points Cantidad de medidas a promediar
	 * @return Corriente promedio en amperes.
	 */
	float getCurrent(const uint8_t points);

private:
	Timer timer;
	AnalogIn currentIn;
};

#endif