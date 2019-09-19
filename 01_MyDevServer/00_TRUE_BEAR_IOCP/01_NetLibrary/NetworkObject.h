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
// SOCKET은 64bit 환경에서 64bit이다. 반면 linux에서는 여전히 32bit이다. 이 차이를 위함.
typedef int SOCKET;
#endif

class Endpoint;

// 소켓 클래스
class NetworkObject
{
public:
	static const int MaxReceiveLength = 8192;
	static const int MaxSendLength = 8192;


	SOCKET m_fd; // 소켓 핸들


	// AcceptEx 함수 포인터
	LPFN_ACCEPTEX AcceptEx = NULL;

	// Overlapped I/O나 IOCP를 쓸 때에만 사용됩니다. 현재 overlapped I/O 중이면 true입니다.
	bool m_isReadOverlapped = false;
	bool m_isWriteOverlapped = false;
	// Overlapped receive or accept을 할 때 사용되는 overlapped 객체입니다. 
	// I/O 완료 전까지는 보존되어야 합니다.
	WSAOVERLAPPED m_readOverlappedStruct;
	WSAOVERLAPPED m_writeOverlappedStruct;

	// Receive나 ReceiveOverlapped에 의해 수신되는 데이터가 채워지는 곳입니다.
	// overlapped receive를 하는 동안 여기가 사용됩니다. overlapped I/O가 진행되는 동안 이 값을 건드리지 마세요.
	char m_receiveBuffer[MaxReceiveLength];
	char m_sendBuffer[MaxSendLength];


	// overlapped 수신을 하는 동안 여기에 recv의 flags에 준하는 값이 채워집니다. overlapped I/O가 진행되는 동안 이 값을 건드리지 마세요.
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

