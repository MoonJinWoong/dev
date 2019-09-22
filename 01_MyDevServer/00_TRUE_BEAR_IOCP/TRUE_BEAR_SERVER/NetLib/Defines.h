#pragma once


enum class MsgType : INT8
{
	None = 0,
	Session,
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


struct Message
{
	MsgType Type = MsgType::None;
	char* pContents = nullptr;

	void Clear()
	{
		Type = MsgType::None;
		pContents = nullptr;
	}

	void SetMessage(MsgType msgType, char* pSetContents)
	{
		Type = msgType;
		pContents = pSetContents;
	}
};



const int MAX_IP_LENGTH = 20;
const int MAX_ADDR_LENGTH = 64;


const int SESSION_MAX_SEND_BUFF_SIZE = 1024;
const int SESSION_MAX_RECV_BUFF_SIZE = 1024;
const int MAX_RECV_OVERLAPPED_BUFF_SIZE = 512;
const int MAX_SEND_OVERLAPPED_BUFF_SIZE = 512;

const int MAX_SESSION_COUNT = 1000;
const int MAX_MESSAGE_POOL_COUNT = 1000;

// CPU에 따라 조절해야함. 나중에 테스트해볼 때 건드리자
const int WORKER_THREAD_COUNT = 4;