/**
 * @file OnOffByTime.h
 * @author Agustin Allegui (a.allegui@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-05-03
 * 
 * @copyright Copyright (c) 2021
 * 
 */


#ifndef ON_OFF_BY_TIME_H
#define ON_OFF_BY_TIME_H

#include <stdint.h>
#include "MTSLog.h"
#include "mbed.h"


class OnOffByTime {
public:

    /**
     * @brief Construct a new On Off By Time object
     * 
     * @param onHour Hora de encendido (0 - 23)
     * @param onMinute Minuto de encendido (0 - 59)
     * @param offHour Hora de apagado (0 - 23)
     * @param offMinute Minuto de encendido (0 - 59)
     */
    OnOffByTime(const uint8_t onHour, const uint8_t onMinute, const uint8_t offHour, const uint8_t offMinute);

    /**
     * @brief Configura las horas de encendido y apagado
     * 
     * @param onHour Hora de encendido (0 - 23)
     * @param onMinute Minuto de encendido (0 - 59)
     * @param offHour Hora de apagado (0 - 23)
     * @param offMinute Minuto de encendido (0 - 59)
     */
    void setOnOffTime(const uint8_t onHour, const uint8_t onMinute, const uint8_t offHour, const uint8_t offMinute);

    /**
     * @brief Obtiene el nivel de dimming segun la hora actual
     * 
     * @param hour Hora actual (0 - 23)
     * @param minute Minuto actual (0 - 59)
     * @return float Nivel de dimming segun la hora (0.0f / 1.0f)
     */
    float getDimming(const uint8_t hour, const uint8_t minute);

    uint8_t getOnHour()
        { return onHour_;}

    uint8_t getOnMinute()
        { return onMinute_;}

    uint8_t getOffHour()
        { return offHour_;}

    uint8_t getOffMinute()
        { return offMinute_;}


protected:
    uint8_t onHour_;
    uint8_t onMinute_;
    uint8_t offHour_;
    uint8_t offMinute_;

};

#endif