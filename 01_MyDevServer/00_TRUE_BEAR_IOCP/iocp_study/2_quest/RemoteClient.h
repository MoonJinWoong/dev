#pragma once
const int MAX_SOCKBUF = 1024;	//패킷 크기
const int MAX_WORKERTHREAD = 4;  //쓰레드 풀에 넣을 쓰레드 수

enum class IOOperation
{
	RECV,
	SEND
};

//WSAOVERLAPPED구조체를 확장 시켜서 필요한 정보를 더 넣었다.
struct CustomOverEx
{
	WSAOVERLAPPED m_wsaOverlapped;		//Overlapped I/O구조체
	SOCKET		m_socketClient;			//클라이언트 소켓
	WSABUF		m_wsaBuf;				//Overlapped I/O작업 버퍼

	char		m_RecvBuf[MAX_SOCKBUF]; //데이터 버퍼
	char		m_SendBuf[MAX_SOCKBUF]; //데이터 버퍼
	IOOperation m_eOperation;			//작업 동작 종류
};

//클라이언트 정보를 담기위한 구조체
class RemoteClient
{
public:
	SOCKET			stRemoteSock;			//Cliet와 연결되는 소켓
	CustomOverEx	stRecvOver;	//RECV Overlapped I/O작업을 위한 변수
	CustomOverEx	stSendOver;	//SEND Overlapped I/O작업을 위한 변수

	RemoteClient()
	{
		ZeroMemory(&stRecvOver, sizeof(CustomOverEx));
		ZeroMemory(&stSendOver, sizeof(CustomOverEx));
		stRemoteSock = INVALID_SOCKET;
	}
};