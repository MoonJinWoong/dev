#pragma once
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib,"mswsock.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#include <WinSock2.h> // memo - window.h 보다 위에 있어야함
#include <Windows.h>
#include <WS2tcpip.h>
#include <iostream>
#include <thread>
#include <string>
#include <mutex>
#include <vector>
#include <list>
#include <unordered_map>
#include <atomic>
#include <concurrent_queue.h>
#include <exception>



// 순서 생각해서 선언해야함
#include "CustomException.h"
#include "SpinLock.h"
#include "CircleBuffer.h"		// 버퍼풀
#include "Defines.h"			// 구조체 정의들
#include "MessagePool.h"
#include "CustomSocket.h"
#include "Session.h"			// 통신 모음
#include "Iocp.h"				// iocp 함수 모음
#include "IocpService.h"
