//-------------------------------------------------------------
// Configuraciones sobre LoRa

#ifndef _LORA_CONFIG_H_
#define _LORA_CONFIG_H_

// the active channel plan is the one that will be compiled
// options are :
//      CP_US915
//      CP_AU915
//      CP_EU868
//      CP_KR920
//      CP_AS923
//      CP_AS923_JAPAN
//      CP_IN865
#if !defined(CHANNEL_PLAN)
#define CHANNEL_PLAN CP_AU915
#endif

//-------------------------------------------------------------
// Habilitar Join

// 0: deactivated
// 1: activated
#define ENABLE_JOIN 1

//-------------------------------------------------------------
// define App Eui, App key y Sub band

#define APP_EUI {0x88, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0x02}

// nodo xDot
#define APP_KEY {0x1d, 0xbf, 0x53, 0x7d, 0xc8, 0xb3, 0xa0, 0x5b, 0x51, 0xd4, 0x39, 0x34, 0xce, 0x6b, 0x8e, 0xe5}

/*	sub band:
 *	0: Auto
 *	1: Yeap
 *	2: ChirpStack
 */

#define LORA_SUB_BAND 1

// define parameters for ABP
#define ABP_ADDRESS     { 0x01, 0x02, 0x03, 0x04 }  // se reemplza por la parte baja del devEUI
#define ABP_NET_KEY     { 0x02, 0xc4, 0x83, 0xf4, 0xf1, 0x18, 0x7e, 0x97, 0xa1, 0xa3, 0x6c, 0x50, 0xa0, 0x0e, 0xad, 0x9b }
#define ABP_DATA_KEY    { 0xc2, 0x6b, 0x38, 0x6d, 0xcf, 0xa4, 0x77, 0xf4, 0x03, 0xe9, 0x8e, 0x07, 0x26, 0x74, 0x7c, 0x71 }

//-------------------------------------------------------------
// join mode

#define LORA_MODE_OTA 1
#define LORA_MODE_ABP 2

#define LORA_JOIN_MODE  LORA_MODE_ABP



//-------------------------------------------------------------
// join atempts

#define INITIAL_JOIN_ATEMPTS 4
#define LOOP_JOIN_ATEMPTS	4

//#define INITIAL_JOIN_ATEMPTS 4
//#define LOOP_JOIN_ATEMPTS	3

#endif
