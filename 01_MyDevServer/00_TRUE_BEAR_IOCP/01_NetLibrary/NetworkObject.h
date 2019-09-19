#pragma once

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#include <mswsock.h>
#else 
#include <sys/socket.h>
#endif


#include <string>


#ifndef _WIN32
// SOCKET�� 64bit ȯ�濡�� 64bit�̴�. �ݸ� linux������ ������ 32bit�̴�. �� ���̸� ����.
typedef int SOCKET;
#endif

class Endpoint;

// ���� Ŭ����
class NetworkObject
{
public:
	static const int MaxReceiveLength = 8192;
	static const int MaxSendLength = 8192;


	SOCKET m_fd; // ���� �ڵ�


	// AcceptEx �Լ� ������
	LPFN_ACCEPTEX AcceptEx = NULL;

	// Overlapped I/O�� IOCP�� �� ������ ���˴ϴ�. ���� overlapped I/O ���̸� true�Դϴ�.
	bool m_isReadOverlapped = false;
	bool m_isWriteOverlapped = false;
	// Overlapped receive or accept�� �� �� ���Ǵ� overlapped ��ü�Դϴ�. 
	// I/O �Ϸ� �������� �����Ǿ�� �մϴ�.
	WSAOVERLAPPED m_readOverlappedStruct;
	WSAOVERLAPPED m_writeOverlappedStruct;

	// Receive�� ReceiveOverlapped�� ���� ���ŵǴ� �����Ͱ� ä������ ���Դϴ�.
	// overlapped receive�� �ϴ� ���� ���Ⱑ ���˴ϴ�. overlapped I/O�� ����Ǵ� ���� �� ���� �ǵ帮�� ������.
	char m_receiveBuffer[MaxReceiveLength];
	char m_sendBuffer[MaxSendLength];


	// overlapped ������ �ϴ� ���� ���⿡ recv�� flags�� ���ϴ� ���� ä�����ϴ�. overlapped I/O�� ����Ǵ� ���� �� ���� �ǵ帮�� ������.
	DWORD m_readFlags = 0;
	DWORD m_writeFlag = 0;


	NetworkObject();
	NetworkObject(SOCKET fd);
	~NetworkObject();

	void BindAndListen(const Endpoint& endpoint);
	int EchoSend(const char* data, int length);
	void Close();
	bool AcceptOverlapped(NetworkObject& acceptCandidateSocket, std::string& errorText);
	int  FinishAcceptEx(NetworkObject& listenSocket);


	int ReceiveOverlapped();

	void SetNonblocking();
	
};

std::string GetLastErrorAsString();

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "mswsock.lib")

