//-------------------------------------------------------------
// Activar lectura de Configuracon de memoria no volatil

#ifndef _VERSION_CONFIG_H_
#define _VERSION_CONFIG_H_

//-------------------------------------------------------------
// Version

#define PROMATIX_VERSION_MAJOR 2
#define PROMATIX_VERSION_MINOR 6
#define PROMATIX_VERSION_PATCH 4

//-------------------------------------------------------------
// Direcciones en memoria no volatil

#define DIR_PROMATIX_VERSION_MAJOR 0x00 // 1 Byte
#define DIR_PROMATIX_VERSION_MINOR 0x01 // 1 Byte
#define DIR_PROMATIX_VERSION_PATCH 0x02 // 1 Byte

#define DIR_INITIAL_NVM_PARAM 0x10 // direccion del primer parametro de memoria no volatil

#define DIR_LOOP_DELAY      DIR_INITIAL_NVM_PARAM       // 3 Bytes
#define DIR_OP_MODE         DIR_INITIAL_NVM_PARAM+3     // 1 Byte
#define DIR_CURVE           DIR_INITIAL_NVM_PARAM+4     // 1 Byte
#define DIR_MANUAL_DIMMING  DIR_INITIAL_NVM_PARAM+5     // 1 Byte
#define DIR_ON_OFF_TIME     DIR_INITIAL_NVM_PARAM+6     // 4 bytes

#define DIR_CUSTOM_CURVE    DIR_INITIAL_NVM_PARAM+10    // 12 Bytes

#define NVM_SAVE_BUFFER_SIZE 10 // tamaño de memoria no volatil sin incluir custom curve

//-------------------------------------------------------------
// Actives generation of test data (para version PULSE_METER)

// 0: deactivated
// 1: activated
#if !defined(TEST_DATA_GEN)
#define TEST_DATA_GEN 0
#endif

#endif

//-------------------------------------------------------------
// 

//-------------------------------------------------------------
// secuencias para los leds

#define LED_SEQUENCE_OK \
    {                   \
        0.2, 2.7        \
    }
#define LED_SEQUENCE_ERROR_1 \
    {                        \
        0.2, 0.5, 0.2, 3.0   \
    }
#define LED_SEQUENCE_ERROR_2         \
    {                                \
        0.2, 0.5, 0.2, 0.5, 0.2, 3.0 \
    }
#define LED_SEQUENCE_ERROR_3                   \
    {                                          \
        0.2, 0.5, 0.2, 0.5, 0.2, 0.5, 0.2, 3.0 \
    }
