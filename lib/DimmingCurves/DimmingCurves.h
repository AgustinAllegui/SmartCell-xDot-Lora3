/**
 * @file DimmingCurves.h
 * @author Agustin Allegui (a.allegui@gmail.com)
 * @brief Gestor de curvas de dimming.
 * @version 0.1
 * @date 2020-09-12
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef _DIMMING_CURVES_H_
#define _DIMMING_CURVES_H_

#include <stdint.h>

/**
 * @brief Curva personalizada por el usuario.
 * 
 * @note Como debe estar almacenada en EPROM, se gestiona
 * externamente pero debe poder accederse desde aca.
 * 
 * La curva personalizada se encuentra al final de las curvas estaticas (curves[][12]).
 * Ejemplo: Si estan definidas 3 curvas estaticas, la curva personalizada tiene el indice 3.
 */
extern uint8_t customCurve[12];

class DimmingCurves
{
public:
  /**
 * @brief Construct a new Dimming Curves object.
 * 
 * @param index Curva inicial seleccionada.
 */
  DimmingCurves(const uint8_t index = 0);

  /**
 * @brief Selecciona la curva a utilizar.
 * 
 * @param index Curva seleccionada.
 * @return Curva seleccionada.
 */
  uint8_t selectCurve(const uint8_t index);

  /**
 * @brief Obtener la curva seleccionada.
 * 
 * @return Curva seleccionada.
 */
  uint8_t getCurrentCurve() { return currentCurve; }

  /**
   * @brief Obtener el nivel de dimming actual segun la curva seleccionada.
   * 
   * @param hour Hora actual (0 - 23).
   * @return Nivel de dimming actual (0 - 1).
   */
  float getDimming(const uint8_t hour);

protected:
  uint8_t currentCurve;
  uint8_t staticCurves;

  static const uint8_t curves[][12];
};

#endif