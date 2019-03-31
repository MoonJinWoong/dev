#pragma once

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>

#include "NetworkFrame.h"
namespace NetworkLib
{
	class selectNetwork 
	{
		selectNetwork();
		~selectNetwork();
		bool Init(NetworkLib::ServerOption opt);

	};
}