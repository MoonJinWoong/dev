#pragma once


// 4πŸ¿Ã∆Æ 
struct Head
{
	int id;
};

struct Body
{
	char buf[512 + 1];
};

#pragma push(1)
struct Packet
{
	int UserID = 0;
	short Operation = 0;
	short bodySize = 0;
	char data[512];
};
#pragma pop