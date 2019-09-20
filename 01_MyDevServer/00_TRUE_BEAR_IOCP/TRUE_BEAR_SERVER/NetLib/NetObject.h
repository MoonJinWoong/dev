#pragma once
#include "NetPreCompile.h"

class NetObject
{
public:
	NetObject();
	~NetObject();
	void MakeSocket();
	void MakeSocket(SOCKET socket);
	bool BindAndListen(const EndAddress& endAddr);
public:
	SOCKET mSock;
};