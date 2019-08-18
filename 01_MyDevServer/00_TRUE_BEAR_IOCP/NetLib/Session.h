#pragma once
#include "PreComplie.h"
const int	MAX_BUFFER = 1024;
const int   MAX_IO_TYPE = 2;


typedef enum
{
	IO_READ,IO_WRITE,IO_ERROR
}IO_TYPE;

struct CLIENT_SOCKET
{
	SOCKET sSocket;
	SOCKADDR_IN sAddrInfo;
};

struct IoData
{
	WSAOVERLAPPED	overlapped;
	IO_TYPE			ioType;
	WSABUF			dataBuf;
	SOCKET			socket;
	//char			messageBuffer[MAX_BUFFER];
	array<char, MAX_BUFFER>  messageBuffer;
	int				totalByte;
	int				currByte;

	void Init();
	void Clear();
	void setType(IO_TYPE type);
	bool isCompleteRecv(int size);

};

class Session
{
public:
	CLIENT_SOCKET					mSockData;
	array<IoData,MAX_IO_TYPE>		mIoData;		// read , write용으로 들고 있어야함
	
	int				m_ID;
	int				mIotype;
	bool			setSockOpt();

public:
	Session();
	void runSend(int size);
	void sendMsg();  // 파라미터에 패킷 

	void runRecv(int size); // 반환형 패키지 
	void recvReady();
private:
	void Init();
	void isError(DWORD errCode);
	void recv(WSABUF wsabuf);
	bool checkReceive(int size);
	void send(WSABUF wsabuf);

};