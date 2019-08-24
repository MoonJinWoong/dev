#pragma once
#include "PreComplie.h"


// �������� ����ü�� ���Ŀ� Ŭ����ȭ �� ��
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




	SOCKET mhSock; // ���� �ڵ�
	// �б� ����
	EXOverlapped mReadOverLappedEx;
private:

};