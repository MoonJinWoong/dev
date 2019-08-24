#pragma once
#include "PreComplie.h"


// 오버랩드 구조체는 추후에 클래스화 할 것
const int MAX_BUFFER = 1024;
struct EXOverlapped
{
	WSAOVERLAPPED	overlapped;
	WSABUF			dataBuf;
	SOCKET			socket;
	char			messageBuffer[MAX_BUFFER];
	int				recvBytes;
	int				sendBytes;
};

class NetTool
{
public:
	NetTool(SocketType type);
	~NetTool();
	void bindAndListen(const string& ip , int port);




	SOCKET mhSock; // 소켓 핸들
	// 읽기 전용
	EXOverlapped mReadOverLappedEx;
private:

};