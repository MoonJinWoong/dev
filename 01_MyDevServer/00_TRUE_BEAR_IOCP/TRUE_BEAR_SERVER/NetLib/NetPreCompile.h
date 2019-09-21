#pragma once
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib,"mswsock.lib")

#include <WinSock2.h> // memo - window.h 보다 위에 있어야함
#include <Windows.h>
#include <WS2tcpip.h>
#include <iostream>
#include <thread>
#include <string>

#include "Defines.h"
#include "EndAddress.h"
#include "NetObject.h"  // 통신 모음
#include "Iocp.h"       // iocp 함수 모음