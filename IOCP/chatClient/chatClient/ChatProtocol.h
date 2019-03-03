#pragma once


#pragma warning( disable:4786 )
#pragma warning( disable:4251 )
#pragma warning( disable:4311 )
#pragma warning( disable:4244 )
#pragma warning( disable:4996 )


#include <iostream>
#include <mutex>
#include <WinSock2.h>
#include <mutex>
#include <thread>
#include <process.h>  // beginethreadEx
#include <Mswsock.h>  // acceptEx 
#include <map>				// connection에서 사용
#include <unordered_map>	// 혹시 몰라서 만들어둠

#include <stdlib.h>


#include <stdio.h>
#include <string.h>


using namespace std;


#pragma comment(lib, "ws2_32")
#pragma comment(lib, "mswsock.lib")  


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
