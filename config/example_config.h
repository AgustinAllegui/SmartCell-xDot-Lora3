#ifndef __EXAMPLE__CONFIG_H__
#define __EXAMPLE__CONFIG_H__

#include "lora_config.h"
#include "version_config.h"

//-------------------------------------------------------------
// Seleccion de version

#define OTA_EXAMPLE 1          // see ota_example.cpp
#define AUTO_OTA_EXAMPLE 2     // see auto_ota_example.cpp
#define MANUAL_EXAMPLE 3       // see manual_example.cpp
#define PEER_TO_PEER_EXAMPLE 4 // see peer_to_peer_example.cpp
#define CLASS_C_EXAMPLE 5      // see class_c_example.cpp
#define CLASS_B_EXAMPLE 6      // see class_b_example.cpp
#define FOTA_EXAMPLE 7         // see fota_example.cpp

#define PULSE_METER 8 // see pulse_meter.cpp
#define LUMINARY 9    // see luminary.cpp

// the active example is the one that will be compiled
#if !defined(ACTIVE_EXAMPLE)
#define ACTIVE_EXAMPLE LUMINARY
#endif

//-------------------------------------------------------------
// Time zone
//! revisar si al sincronizarse toma la hora del gateway segun la zona horaria o toma siempre la UTC
#define TIME_ZONE -3

//-------------------------------------------------------------
#endif
