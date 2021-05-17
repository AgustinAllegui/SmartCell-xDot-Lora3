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

//-------------------------------------------------------------
// join atempts

#define INITIAL_JOIN_ATEMPTS 4
#define LOOP_JOIN_ATEMPTS	4

//#define INITIAL_JOIN_ATEMPTS 4
//#define LOOP_JOIN_ATEMPTS	3

#endif
