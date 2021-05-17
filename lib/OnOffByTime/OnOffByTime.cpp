/**
 * @file OnOffByTime.c
 * @author Agustin Allegui (a.allegui@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-05-03
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "OnOffByTime.h"


OnOffByTime::OnOffByTime(const uint8_t onHour, const uint8_t onMinute, const uint8_t offHour, const uint8_t offMinute)
    :onHour_(onHour)
    ,onMinute_(onMinute)
    ,offHour_(offHour)
    ,offMinute_(offMinute)
{

}

void OnOffByTime::setOnOffTime(const uint8_t onHour, const uint8_t onMinute, const uint8_t offHour, const uint8_t offMinute)
{
    onHour_ = onHour;
    onMinute_ = onMinute;
    offHour_ = offHour;
    offMinute_ = offMinute;
}

float OnOffByTime::getDimming(const uint8_t hour, const uint8_t minute)
{
    logDebug("Current time %02u:%02u", hour, minute);
    // expreso todas las horas en minutos desde las 00:00
    uint16_t absMinuteOn = onHour_ * 60 + onMinute_;
    uint16_t absMinuteOff = offHour_ * 60 + offMinute_;
    uint16_t absMinuteCurrent = hour * 60 + minute;

    // si la hora off es menor que la hora on, agregar 24 horas para tener el rango sin cortar
    if(absMinuteOff < absMinuteOn){
        absMinuteOff += 24*60;
        absMinuteCurrent += 24*60;
    }

    float dimming = 0.0;

    if((absMinuteCurrent <= absMinuteOff) && (absMinuteCurrent >= absMinuteOn)){
        dimming = 1.0;
    }

    return dimming;

}