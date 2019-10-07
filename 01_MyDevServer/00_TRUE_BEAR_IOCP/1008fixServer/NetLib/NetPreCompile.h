#pragma once
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib,"mswsock.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#include <WinSock2.h> // memo - window.h ���� ���� �־����
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




// ���� �����ؼ� �����ؾ���
#include "SpinLock.h"
#include "CircleBuffer.h"		// ����Ǯ
#include "Defines.h"			// ����ü ���ǵ�
#include "CustomSocket.h"
#include "CustomOverlapped.h"
#include "Session.h"			// ��� ����

#include "Iocp.h"				// iocp �Լ� ����
#include "IocpService.h"
