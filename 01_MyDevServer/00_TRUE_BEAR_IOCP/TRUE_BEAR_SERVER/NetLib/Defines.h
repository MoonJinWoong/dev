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

const int SESSION_MAX_SEND_BUFF_SIZE = 30720;
const int SESSION_MAX_RECV_BUFF_SIZE = 30720;
const int MAX_RECV_OVERLAPPED_BUFF_SIZE = 512;
const int MAX_SEND_OVERLAPPED_BUFF_SIZE = 512;

const int MAX_SESSION_COUNT = 1000;
const int MAX_MESSAGE_POOL_COUNT = 1000;

// CPU�� ���� �����ؾ���. ���߿� �׽�Ʈ�غ� �� �ǵ帮��
const int WORKER_THREAD_COUNT = 4;

// GQCSEx���� �ѹ��� � �����ðų�
static const int MAX_EVENT_COUNT = 1000;  

// TODO : ���� �����غ���
const int MAX_MSG_POOL_COUNT = 100;
const int EXTRA_MSG_POOL_COUNT = 100;


const int PACKET_HEADER_LENGTH = 5;
const int PACKET_SIZE_LENGTH = 2;
const int PACKET_TYPE_LENGTH = 2;