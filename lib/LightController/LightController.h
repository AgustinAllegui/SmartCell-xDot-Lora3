/**
 * @file LightController.h
 * @author Agustin Allegui (a.allegui@gmail.com)
 * @brief Controlador de luminaria con varios modos de operacion.
 * @version 0.1
 * @date 2020-09-11
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef _LIGHT_CONTROLLER_H_
#define _LIGHT_CONTROLLER_H_

#include "mbed.h"
#include "MTSLog.h"
#include "PhotoCell.h"
#include "OnOffByTime.h"
#include "DimmingCurves.h"
#include "DimmingDemo.h"

class LightController
{
public:
  enum class OpMode : uint8_t
  {
    Manual = 0x00,
    AutoPhotoCell = 0x01,
    AutoCurve = 0x02,
    Demo = 0x03,
    AutoTime = 0x04
  };

  /**
   * @brief Construct a new Light Controller object
   * 
   * @param photoCell_ Puntero a manejado de fotocelda
   * @param dimmingCurves_ Puntero a manejador de curvas
   * @param onOffByTime_ Puntero para manejo de on off por tiempo
   * @param dimmingDemo_ Puntero a manejado de demo
   * @param opMode_ Modo de operacion inicial
   */
  LightController(  PhotoCell *photoCell_,
                    DimmingCurves *dimmingCurves_,
                    OnOffByTime *onOffByTime_,
                    DimmingDemo *dimmingDemo_,
                    const LightController::OpMode opMode_);

  /**
   * @brief Obtiene el valor de dimming segun la hora y modo actual.
   * 
   * @param hour Hora actual (0 - 23).
   * @param minute Minuto actual (0 - 59).
   * @return Valor de dimming actual (0 - 1).
   */
  float getDimming(const uint8_t hour, const uint8_t minute);

  /**
   * @brief Configura el valor de dimming manual.
   * 
   * @param dimLevel Valor de dimming a configurar.
   */
  void setManualDimming(float dimLevel);

  /**
   * @brief Configura el modo de operacion.
   * 
   * @param opMode_ Modo de operacion.
   */
  void setOpMode(const OpMode opMode_);

  /**
   * @brief Obtiene el modo de operacion.
   * 
   * @return Modo de operacion casteado a uint8_t 
   */
  uint8_t getMode();

  /**
   * @brief Imprime por consola el modo de operacion actual
   * 
   */
  void printMode();

  float getManualDimLevel();

private:
  PhotoCell *photoCell;
  DimmingCurves *dimmingCurves;
  OnOffByTime *onOffByTime;
  DimmingDemo *dimmingDemo;
  OpMode currentMode;

  float manualDimLevel;
};

#endif
