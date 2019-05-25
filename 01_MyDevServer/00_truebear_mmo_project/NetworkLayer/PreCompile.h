

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>


namespace NetworkLayer
{
	const int SERVER_PORT = 9000;
}
