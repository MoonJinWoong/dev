#pragma once
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib,"mswsock.lib")

#include <WinSock2.h> // memo - window.h ���� ���� �־����
#include <Windows.h>
#include <WS2tcpip.h>
#include <iostream>
#include <thread>
#include <string>

#include "Defines.h"
#include "EndAddress.h"
#include "NetObject.h"  // ��� ����
#include "Iocp.h"       // iocp �Լ� ����