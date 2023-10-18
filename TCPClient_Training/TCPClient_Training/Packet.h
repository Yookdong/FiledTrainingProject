#pragma once

#ifndef __PACKET_H__ 
#define __PACKET_H__

enum class EPacket
{
	None = 0,

	S2C_CastMessage = 1,
	C2S_CastMessage = 2,
};

#pragma pack(1)

typedef struct _PacketQuitClient
{
	char* UserID;
} PacketQuitClient;

#pragma pack()

#endif