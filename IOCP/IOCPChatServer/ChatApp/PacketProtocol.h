#pragma once

#define MAX_CHATMSG 300
#define MAX_IP		20

enum Packet {

	PACKET_CHAT = 1,
};

struct PacketChat
{
	unsigned int	sLength;
	unsigned short	sType;
	char			sIP[MAX_IP];
	char			sChatMsg[MAX_CHATMSG];
};
