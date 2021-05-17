/**
 * @file smartCell_util.h
 * @brief Funciones utiles para SmartCell
 * @version 0.1
 * @date 2020-08-26
 * 
 * @copyright Copyright (c) 2020
 * 
 */

// SmartCell_util.h

#ifndef SMART_CELL_UTIL_H
#define SMART_CELL_UTIL_H

#include "example_config.h"
#include "mbed.h"
#include "mDot.h"
#include "dot_util.h"

//#include "EventSlot.h"

// declaro el objeto dot para tener acceso desde las funciones
extern mDot *dot;

// Funciones para Manejo del RTC

/**
 * @brief Devuelve true si la hora se sincronizo.
 * 
 * verifica que el año actual es mayor a 2018.
 * 
 * @return true 
 * @return false 
 */
bool timeIsSynced();

/**
 * @brief Sincroniza la hora usando la red LoRa.
 * 
 * @note Utiliza el comando MAC devTimeReq.
 * 
 * @param attempts Maxima cantidad de intentos de sincronizacion.
 * @param timezone Zona horaria.
 * @return true - La sincronizacion se realizo con exito.
 */
bool syncTime(const uint8_t attempts = 1, const float timezone = TIME_ZONE);

/**
 * @brief Devuelve unicamente el hora actual (0 - 23).
 * 
 * @return uint8_t (0 - 23)
 */
uint8_t getHour();

/**
 * @brief Devuelve unicamente el minuto actual (0 - 59).
 * 
 * @return uint8_t 
 */
uint8_t getMinute();

/**
 * @brief Setea el RTC a una hora especifica
 * 
 * @param timestamp Segundos desde el UNIX epoch
 */
void setManualTime(const uint32_t timestamp);

/**
 * @brief Imrime por consola el tiempo actual.
 * 
 * @note Utiliza nivel de log INFO
 */
void printTime();

//---------------------------------------------------------------------

// funciones para envio de datos

/**
 * @brief Si esta unico, envia dimming, power y energy.
 * 
 * @param dimming valor de dimming (0 - 1)
 * @param power valor de potencia (>0)
 * @param energy valor de potencia (>0)
 */
bool send_lightStatus(const float dimming, const float power, const float energy, const uint8_t mode);

/**
 * @brief Si esta unico, envia las configuraciones actuales del nodo.
 * 
 * (modo, valor de dimming manual, retardo entre ciclos)
 * 
 * @param lightMode Modo de funcionamiento.
 * @param manualDimming Nivel de dimming manual.
 * @param loopDelay Retardo entre ciclos [segundos]
 */
bool send_smartCellConfig(const uint8_t lightMode, const uint8_t manualDimming, const uint16_t loopDelay, const uint8_t loopsToSend);

/**
 * @brief Si esta unico, envia el timestamp actual.
 * 
 */
bool send_currentTime();

#endif