#pragma once


// winsock2 ����� ���� �Ʒ� �ڸ�Ʈ �߰�
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <Mswsock.h> //acceptEx
#include <Ws2tcpip.h>
#include <string>
#include <process.h>
#include <iostream>
#include <thread>
#include <vector>
#include <array>
#include <list>
#include <mutex>
#include <algorithm>
#include <memory>


enum class SocketType
{
	TCP,
	UDP,
};

using namespace std;


#include "NetTool.h"
#include "Thread.h"


const int   SERVER_PORT = 9000;

