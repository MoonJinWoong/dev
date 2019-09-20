#pragma once
#include "NetPreCompile.h"



//EndAddress EndAddress::Any;

class EndAddress
{
public:
	EndAddress()
	{
		memset(&m_ipv4EndAddr, 0, sizeof(m_ipv4EndAddr));
		m_ipv4EndAddr.sin_family = AF_INET;
	}
	EndAddress(const char* addr, int port)
	{
		memset(&m_ipv4EndAddr, 0, sizeof(m_ipv4EndAddr));
		m_ipv4EndAddr.sin_family = AF_INET;
		inet_pton(AF_INET, addr, &m_ipv4EndAddr.sin_addr);
		m_ipv4EndAddr.sin_port = htons((uint16_t)port);
	}
	~EndAddress()
	{

	}

public:
	sockaddr_in m_ipv4EndAddr;
	static EndAddress Any;

};


