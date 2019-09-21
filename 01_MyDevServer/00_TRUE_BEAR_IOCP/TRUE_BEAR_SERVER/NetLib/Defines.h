#pragma once


enum class MsgType : INT8
{
	None = 0,
	Connection,
	Close,
	OnRecv
};


enum class OPType : INT8
{
	None = 0,
	Send,
	Recv,
	Accept,
};

struct CustomOverlapped
{
	OVERLAPPED Overlapped;
	WSABUF OverlappedExWsaBuf;

	OPType type;

	int	totalByte;
	DWORD OverlappedExRemainByte;
	char* pOverlappedExSocketMessage;

	INT32 SessionIdx = 0;

	CustomOverlapped(INT32 connectionIndex)
	{
		ZeroMemory(this, sizeof(CustomOverlapped));
		SessionIdx = connectionIndex;
	}
};