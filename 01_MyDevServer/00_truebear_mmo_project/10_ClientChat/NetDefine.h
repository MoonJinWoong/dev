//#pragma once


#define _CRT_SECURE_NO_WARNINGS         // 최신 VC++ 컴파일 시 경고 방지
#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
//#include <Windows.h>
//#include <tchar.h>
#include <stdlib.h>
#include <stdio.h>
#include "resource.h"
#include "../03_PacketDefine/Packet.h"
#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    512
#define	WM_SOCKET				WM_USER + 1



// 로그인 입력 받을 공간
const int input_size = 16;

const int MAX_PACKET_SIZE = 512;
const int MAX_SOCK_RECV_BUFFER = 8016;