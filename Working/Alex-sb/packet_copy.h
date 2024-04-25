#ifndef __CONTROL_H__
#define __CONTROL_H__

#include <stdint.h>

#define MAX_STR_LEN   32
// This packet has 1 + 1 + 2 + 32 + 16 * 4 = 100 bytes
// Packet now has 36 bytes, no need for params
typedef struct
{

	char packetType;
	char command;
	char dummy[2]; // Padding to make up 4 bytes
	char data[MAX_STR_LEN]; // String data
	// uint32_t params[16];
} TPacket;


#endif
