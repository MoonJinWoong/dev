//#pragma once


#define _CRT_SECURE_NO_WARNINGS         // �ֽ� VC++ ������ �� ��� ����
#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
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



// �α��� �Է� ���� ����
const int input_size = 16;

const int MAX_PACKET_SIZE = 512;
const int MAX_SOCK_RECV_BUFFER = 8016;